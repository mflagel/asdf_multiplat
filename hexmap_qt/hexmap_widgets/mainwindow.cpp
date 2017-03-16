#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QQuickView>
#include <QGLFormat>
#include <QListView>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>

#include <memory>

#include <asdf_multiplat/main/asdf_multiplat.h>
#include <asdf_multiplat/data/content_manager.h>
#include <hexmap/data/spline.h>

#include <glm/gtc/matrix_transform.hpp>

#include "palette_widget.h"
#include "spline_settings_widget.h"
#include "ui_spline_settings_widget.h"
#include "ui_tools_panel.h"
#include "dialogs/new_map_dialog.h"
#include "ui_new_map_dialog.h"

using namespace std;
//using namespace glm;  //causes namespace collision with uint


namespace
{
    constexpr int scroll_sub_ticks = 10;
}

using editor_t = asdf::hexmap::editor::editor_t;
using tool_type_e = editor_t::tool_type_e;
using spline_t = asdf::hexmap::data::spline_t;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Hexmap");
    editor = &(ui->hexmap_widget->editor);


    {
        connect(ui->actionNew,     &QAction::triggered, this, &MainWindow::new_map);
        connect(ui->actionOpen,    &QAction::triggered, this, &MainWindow::open_map);
        connect(ui->actionSave,    &QAction::triggered, this, &MainWindow::save_map);
        connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::save_map_as);

        connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::undo);
        connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::redo);
    }


    // give hexmap widget a pointer to this, so it can set up scrollbar ranges after initializeGL is called
    // can't do it now because we need the hex_map to be created, which requires an openGL context that hasnt
    // been initialized yet
    // EDIT: This may be solvable with the hex_map_initialized signal
    ui->hexmap_widget->main_window = this;
    ui->hexmap_widget->setMouseTracking(true); //enable to get mouseMove events

    connect(ui->hexmap_hscroll, &QScrollBar::valueChanged, this, &MainWindow::scrollbar_changed);
    connect(ui->hexmap_vscroll, &QScrollBar::valueChanged, this, &MainWindow::scrollbar_changed);

    {
        connect(ui->hexmap_widget, &hexmap_widget_t::hex_map_initialized,
                            this, &MainWindow::hex_map_initialized);
        connect(ui->hexmap_widget, &hexmap_widget_t::editor_tool_changed,
                             this, &MainWindow::editor_tool_changed);
        connect(ui->hexmap_widget, &hexmap_widget_t::camera_changed,
                             this, &MainWindow::set_scrollbar_stuff);

        //FIXME cleanup and/or optimize
        connect(ui->hexmap_widget, &hexmap_widget_t::editor_tool_changed,
                [this](tool_type_e tool){
                    if(tool == editor_t::place_splines)
                    {
                        ui->right_dock->setWidget(spline_settings_widget);
                    }
                    else
                    {
                        ui->right_dock->setWidget(palette_widget);
                    }
                }
        );
    }

    {
        auto* tools_ui = ui->tools_panel->ui;
        auto pressed = &QToolButton::pressed;

        connect(tools_ui->SelectTool, pressed, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::select);});
        connect(tools_ui->BrushTool,  pressed, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::terrain_paint);});
        connect(tools_ui->ObjectTool, pressed, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::place_objects);});
        connect(tools_ui->LineTool,   pressed, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::place_splines);});
    }

    {
        palette_widget = new palette_widget_t(this);

        palette_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
        //palette_widget->setMinimumSize(200, 300);
        palette_widget->updateGeometry();

        connect(palette_widget->list_view, SIGNAL(pressed(const QModelIndex&)),
                        ui->hexmap_widget, SLOT(set_palette_item(const QModelIndex&)));


        ui->right_dock->setWidget(palette_widget);
    }


    {
        spline_settings_widget = new spline_settings_widget_t();
        spline_settings_widget->ui->LineThicknessSpinner->setValue(editor->new_node_style.thickness);

        connect(spline_settings_widget->ui->InterpolationDropDown, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
            , [this](int index)
              {
                  ASSERT(index >= 0, "");
                  auto interp = (spline_t::interpolation_e)(index);
                  ui->hexmap_widget->editor.set_current_spline_interpolation(interp);
              });

        connect(spline_settings_widget->ui->LineThicknessSpinner, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [this](double value)
                {
                    auto& editor = ui->hexmap_widget->editor;
                    auto node_style = editor.new_node_style;
                    ASSERT(value <= std::numeric_limits<float>::max(), "spline thickness too large to store in a float");
                    node_style.thickness = static_cast<float>(value);
                    ui->hexmap_widget->editor.set_spline_node_style(node_style);
                });

        connect(spline_settings_widget, &spline_settings_widget_t::colorSelected,
                [this](QColor c) {
                    auto& editor = ui->hexmap_widget->editor;
                    auto node_style = editor.new_node_style;
                    node_style.color = color_t(c.redF(), c.greenF(), c.blueF(), c.alphaF());
                    ui->hexmap_widget->editor.set_spline_node_style(node_style);
                });
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
   ________________
  |   entire map   |
  | ________       |
  ||        |      |
  || screen |      |
  ||________|      |
  |________________|
            ^------^ scroll range
  ^---------^        page step
  ^----------------^ document length
*/
void MainWindow::set_scrollbar_stuff(asdf::camera_t const& camera)
{
    using namespace glm;

    ///VALUES NEEDED:
    ///    "entire_map"     the size of the map in world units
    ///    "viewable_rect"  the rectangle of viewable area in world units

    hexmap_widget_t* hxm_wgt = ui->hexmap_widget;

    ///MAP SIZE
    auto const& map_size_cells = hxm_wgt->map_size(); //array size
    vec2 hex_size(asdf::hexmap::hex_width, asdf::hexmap::hex_height);
    vec2 map_size_units = vec2(map_size_cells) * hex_size; //total map width in units. Width is not equal to array width because hexagons overlap, and hexagons aren't one unit tall
    map_size_units.x -= (map_size_units.x - 1) * asdf::hexmap::hex_width_d4; //handle horizontal overlap
    map_size_units.y += asdf::hexmap::hex_height_d2; //add room for the offset

    ///VIEWABLE RECT
    ///convert the screen coords of the lower-bound and upper-bound of the hexmap rect into world coords
    ///lb and ub are basically {0,0} and {width,height}
    vec2 wgt_size(hxm_wgt->width(), hxm_wgt->height());
    vec3 r_lb = camera.screen_to_world_coord(vec2(0.0f));
    vec3 r_ub = camera.screen_to_world_coord(wgt_size);
    //vec2 viewable_rect_pos = vec2(r_lb + (r_ub - r_lb)/2.0f);
    vec2 viewable_rect_size = vec2(r_ub - r_lb);

    ///SET SCROLLBAR VALUES
    /// these values are in world units
    vec2 scroll_range = glm::max(vec2{0.0f}, map_size_units - viewable_rect_size);
    vec2 scroll_min = glm::min(viewable_rect_size, map_size_units) / 2.0f;
    vec2 scroll_max = scroll_min + scroll_range;
    vec2 page_step = glm::min(viewable_rect_size, map_size_units);

    auto* h_scr = ui->hexmap_hscroll;
    h_scr->setMinimum(scroll_min.x * scroll_sub_ticks);
    h_scr->setMaximum(scroll_max.x * scroll_sub_ticks);
    h_scr->setPageStep(page_step.x * scroll_sub_ticks);

    auto* v_scr = ui->hexmap_vscroll;
    v_scr->setMinimum(scroll_min.y * scroll_sub_ticks);
    v_scr->setMaximum(scroll_max.y * scroll_sub_ticks);
    v_scr->setPageStep(page_step.y * scroll_sub_ticks);
}


void MainWindow::scrollbar_changed()
{
    //LOG("scrollbar changed;  x:%d  y%d", ui->hexmap_hscroll->value(), ui->hexmap_vscroll->value());

    glm::vec2 p(ui->hexmap_hscroll->value());

    auto docheight = ui->hexmap_vscroll->maximum() + ui->hexmap_vscroll->pageStep();
    p.y = docheight - ui->hexmap_vscroll->value(); //since a higher camera pos moves up, but larger scrollbar value should go downward

    p /= scroll_sub_ticks;
    p.x -= asdf::hexmap::hex_width_d2; //since 0,0 is the center of a hex

    ui->hexmap_widget->camera_pos(p, false);
    ui->hexmap_widget->update();
}

void MainWindow::new_map()
{
    if(editor->map_is_dirty)
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();


        switch (ret) {
          case QMessageBox::Save:
              save_map();
              break;
          case QMessageBox::Discard:
              break;
          case QMessageBox::Cancel:
              return;
          default: break;
        }
    }


    new_map_dialog_t nm(this);
    if(!nm.exec()) { //exec() blocks the mainw window until the modal dialog is dismissed
        return;
    } else {
        glm::uvec2 map_size(nm.ui->spb_width->value(), nm.ui->spb_height->value());
        ui->hexmap_widget->editor.new_map_action(map_size);
    }
}

void MainWindow::open_map()
{
    QDir map_file(QString(editor->map_filepath.c_str()));

    QString filepath = QFileDialog::getOpenFileName(this,
        tr("Open Map"), map_file.path(), tr("Hexmap Files (*.hxm)"));

    if(filepath.size() > 0)
    {
        editor->load_action(std::string(filepath.toUtf8().constData()));
    }
}

void MainWindow::save_map()
{
    if(ui->hexmap_widget->editor.map_filepath.size() == 0)
    {
        save_map_as();
    }
    else
    {
        ui->hexmap_widget->editor.save_action();
    }
}

void MainWindow::save_map_as()
{
    QString dir(ui->hexmap_widget->editor.map_filepath.c_str());
    QString filepath = QFileDialog::getSaveFileName(this, tr("Save Map"), dir, tr("Hexmap Files (*.hxm)"));

    if(filepath.size() > 0)
    {
        editor->save_action( std::string(filepath.toUtf8().constData()) );
    }
}

void MainWindow::undo()
{
    editor->undo();
}

void MainWindow::redo()
{
    editor->redo();
}



void MainWindow::mouseMoveEvent(QMouseEvent* event)
{

}

void MainWindow::mousePressEvent(QMouseEvent* event)
{

}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{

}


void MainWindow::hex_map_initialized(asdf::hexmap::editor::editor_t& editor)
{
    terrain_palette_model = new palette_item_model_t();
    objects_palette_model = new palette_item_model_t();

    terrain_palette_model->build_from_terrain_bank(editor.rendered_map->terrain_bank);
    objects_palette_model->build_from_atlas(*(editor.rendered_map->objects_atlas.get()));

    editor_tool_changed(editor.current_tool);
}

void MainWindow::editor_tool_changed(tool_type_e new_tool)
{
    switch(new_tool)
    {
        case tool_type_e::terrain_paint:
            palette_widget->list_view->setModel(terrain_palette_model);
            break;
        case tool_type_e::place_objects:
            palette_widget->list_view->setModel(objects_palette_model);
            break;

    default:
        palette_widget->list_view->setModel(nullptr);
        break;
    }
}
