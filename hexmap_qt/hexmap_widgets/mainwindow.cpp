#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QQuickView>
#include <QGLFormat>
#include <QListView>

#include <memory>

#include <asdf_multiplat/main/asdf_multiplat.h>
#include <asdf_multiplat/data/content_manager.h>

#include <glm/gtc/matrix_transform.hpp>

#include "palette_widget.h"
#include "ui_tools_panel.h"
#include "dialogs/new_map_dialog.h"
#include "ui_new_map_dialog.h"

using namespace std;
//using namespace glm;  //causes namespace collision with uint


namespace
{
    constexpr int scroll_sub_ticks = 10;
}

using tool_type_e = asdf::hexmap::editor::editor_t::tool_type_e;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    {
        connect(ui->actionNew,     &QAction::triggered, this, &MainWindow::new_map);
        connect(ui->actionOpen,    &QAction::triggered, this, &MainWindow::open_map);
        connect(ui->actionSave,    &QAction::triggered, this, &MainWindow::save_map);
        connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::save_map_as);
    }


    // give hexmap widget a pointer to this, so it can set up scrollbar ranges after initializeGL is called
    // can't do it now because we need the hex_map to be created, which requires an openGL context that hasnt
    // been initialized yet
    // EDIT: This may be solvable with the hex_map_initialized signal
    ui->hexmap_widget->main_window = this;

    connect(ui->hexmap_hscroll, &QScrollBar::valueChanged, this, &MainWindow::scrollbar_changed);
    connect(ui->hexmap_vscroll, &QScrollBar::valueChanged, this, &MainWindow::scrollbar_changed);

    {
        connect(ui->hexmap_widget, &hexmap_widget_t::hex_map_initialized,
                            this, &MainWindow::hex_map_initialized);
        connect(ui->hexmap_widget, &hexmap_widget_t::editor_tool_changed,
                             this, &MainWindow::editor_tool_changed);
        connect(ui->hexmap_widget, &hexmap_widget_t::camera_changed,
                             this, &MainWindow::set_scrollbar_stuff);
    }

    {
        auto* tools_ui = ui->tools_panel->ui;
        auto pressed = &QToolButton::pressed;

        connect(tools_ui->SelectTool, pressed, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::select);});
        connect(tools_ui->BrushTool,  pressed, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::terrain_paint);});
        connect(tools_ui->ObjectTool, pressed, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::place_objects);});
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
*/


/// TODO: refactor this such that most of the math is done in the hexmap widget
///       and the widget then calls this function with scrollbar ranges as params
void MainWindow::set_scrollbar_stuff(asdf::camera_t const& camera)
{
    using namespace glm;

    hexmap_widget_t* hxm_wgt = ui->hexmap_widget;

    vec2 hex_size(asdf::hexmap::hex_width, asdf::hexmap::hex_height);

    auto const& map_size = hxm_wgt->map_size(); //array size
    vec2 map_size_units = vec2(map_size) * hex_size; //total map width in units. Width is not equal to array width because hexagons overlap, and hexagons aren't one unit tall
    map_size_units.x -= (map_size.x - 1) * asdf::hexmap::hex_width_d4; //handle horizontal overlap
    map_size_units.y += asdf::hexmap::hex_height_d2; //add room for the offset

    //get size of a hex in pixels. hex_width and hex_height constexpr constants exist in ui/hex_map.h
    vec2 hx_size_px = hex_size * camera.zoom();
    vec2 map_size_px = map_size_units * hx_size_px;

    base_camera_offset = glm::vec2();
    base_camera_offset.x = width()/2 / asdf::hexmap::px_per_unit;
    base_camera_offset.y = map_size_units.y - (height()/2 / asdf::hexmap::px_per_unit);

    hxm_wgt->camera_pos(base_camera_offset, false);

    //range is equal to how many units fit on the screen minus total map size in units

    //map_size_screen_px = unproject map_size_units from camera?

    glm::vec2 wgt_size(hxm_wgt->width(), hxm_wgt->height());


    auto hxm_proj = hxm_wgt->hex_map->camera.projection_ortho();

    vec4 vp;
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.z = wgt_size.x;
    vp.w = wgt_size.y;
    auto unprojd = unProject(vec3(wgt_size, 0.0f), glm::mat4(), hxm_proj, vp);

    auto screen_size_units = glm::vec2(unprojd);
    auto scr_range_units = glm::vec2(map_size_units) - glm::vec2(screen_size_units);

    auto* h_scr = ui->hexmap_hscroll;
    h_scr->setMinimum(0);
    h_scr->setMaximum(scr_range_units.x * scroll_sub_ticks);
    h_scr->setPageStep(screen_size_units.x * scroll_sub_ticks);

    auto* v_scr = ui->hexmap_hscroll;
    v_scr->setMinimum(0);
    v_scr->setMaximum(scr_range_units.y * scroll_sub_ticks);
    v_scr->setPageStep(screen_size_units.y * scroll_sub_ticks);

    scrollbar_changed();
}


void MainWindow::scrollbar_changed()
{
    //LOG("scrollbar changed;  x:%d  y%d", ui->hexmap_hscroll->value(), ui->hexmap_vscroll->value());

    glm::vec2 p(ui->hexmap_hscroll->value(), -ui->hexmap_vscroll->value());
    p /= scroll_sub_ticks;
    p += base_camera_offset;

    ui->hexmap_widget->camera_pos(p, false);
    ui->hexmap_widget->update();
}

void MainWindow::new_map()
{
    new_map_dialog_t nm(this);

    nm.exec(); //supposedly this blocks until the modal dialog is dismissed

    if(!nm.Accepted)
        return;

    //TODO: prompt to save any unsaved changes in the current map
    ui->hexmap_widget->editor.new_map_action(glm::uvec2(nm.ui->spb_width->value(), nm.ui->spb_height->value()));
}

void MainWindow::open_map()
{
}

void MainWindow::save_map()
{
}

void MainWindow::save_map_as()
{
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
