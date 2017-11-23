#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QQuickView>
#include <QGLFormat>
#include <QListView>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDockWidget>

#include <memory>

#include <asdf_multiplat/main/asdf_multiplat.h>
#include <asdf_multiplat/data/content_manager.h>
#include <hexmap/data/hex_grid.h>
#include <hexmap/data/spline.h>
#include <hexmap/ui/hex_map.h>

#include <glm/gtc/matrix_transform.hpp>

#include "palette_widget.h"
#include "spline_settings_widget.h"
#include "ui_spline_settings_widget.h"
#include "ui_tools_panel.h"
#include "dialogs/new_map_dialog.h"
#include "ui_new_map_dialog.h"
#include "object_properties_widget.h"
#include "minimap_widget.h"
#include "terrain_brush_selector.h"
#include "snap_points_widget.h"
#include "tools_panel.h"

using namespace std;
//using namespace glm;  //causes namespace collision with uint
namespace stdfs = std::experimental::filesystem;


namespace
{
    constexpr int scroll_sub_ticks = 10;
    constexpr float scroll_padding_units = 1.0f;
    constexpr int status_message_timeout_ms = 5000;

    constexpr int zoom_exponent_tick_per_press = 1;

    constexpr const char* default_map_name = "untitled";
    constexpr size_t default_map_width  = 30;
    constexpr size_t default_map_height = 30;

    constexpr const char* default_map_file_extension = "hxm";
    constexpr const char* editor_workspace_filename = "hexmap_workspace.json";
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

    hexmap_widget = ui->hexmap_widget;

    {
        connect(ui->actionNew,     &QAction::triggered, this, &MainWindow::new_map);
        connect(ui->actionOpen,    &QAction::triggered, this, &MainWindow::open_map);
        connect(ui->actionSave,    &QAction::triggered, this, &MainWindow::save_map);
        connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::save_map_as);

        connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::undo);
        connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::redo);

        /// Zoom
        {
            auto zoom_tick = [this](int num_ticks)
            {
                auto cur_zoom = ui->hexmap_widget->camera_zoom_exponent();
                ui->hexmap_widget->camera_zoom_exponent(cur_zoom + zoom_exponent_tick_per_press * num_ticks);
            };

            connect(ui->actionZoom_In,  &QAction::triggered, [&zoom_tick](){zoom_tick(1);});
            connect(ui->actionZoom_Out, &QAction::triggered, [&zoom_tick](){zoom_tick(-1);});

            connect(ui->actionZoom_to_Selection, &QAction::triggered, ui->hexmap_widget, &hexmap_widget_t::zoom_to_selection);
            connect(ui->actionZoom_to_Extents,   &QAction::triggered, ui->hexmap_widget, &hexmap_widget_t::zoom_extents);
        }
    }

    {
        zoom_spinbox = new QSpinBox();
        zoom_spinbox->setMinimum(10);
        zoom_spinbox->setMaximum(1600);

        //connect(ui->hexmap_widget, &hexmap_widget_t::camera_changed, [this](asdf::camera_t const& camera){zoom_spinbox->setValue(camera.position.z * 100);});
        connect(zoom_spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int v)
        {
            float z = sqrt(v / 100.0f);
            ui->hexmap_widget->camera_zoom_exponent(z);
        });

        auto* zoomform = new QFormLayout();
        zoomform->addRow("Zoom:", zoom_spinbox);
        statusBar()->addWidget(zoomform->widget());
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
                            this, &MainWindow::init);
        connect(ui->hexmap_widget, &hexmap_widget_t::editor_tool_changed,
                             this, &MainWindow::editor_tool_changed);
        //connect(ui->hexmap_widget, &hexmap_widget_t::camera_changed,
        //                     this, &MainWindow::set_scrollbar_stuff);
    }

    /// Docks
    /// Must pass 'this' to the dock ctor or else it won't lay out properly
    minimap_dock = new QDockWidget(tr("Minimap"), this);
    tool_settings_dock = new QDockWidget(tr("Tool Settings"), this);
    palette_dock = new QDockWidget(tr("Palette"), this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::init()
{
    editor = ui->hexmap_widget->editor.get();
    load_workspace_create_if_not_exists();
    set_recent_documents(editor->workspace.recently_opened);

    /// Render Flag Toggles
    {
        using rflags = asdf::hexmap::ui::hex_map_t::render_flags_e;

        ui->actionGrid->setChecked((editor->map_render_flags & rflags::grid_outline));
        ui->actionHex_Coords->setChecked((editor->map_render_flags & rflags::hex_coords));

        auto connect_thing = [this](QAction* thing, rflags flag)
        {
            connect(thing, &QAction::triggered, [this, flag](bool chk)
            {
                uint32_t temp = static_cast<uint32_t>(ui->hexmap_widget->editor->map_render_flags);
                temp |= (flag * chk);
                temp &= ~(flag * !chk);
                ui->hexmap_widget->editor->map_render_flags = static_cast<rflags>(temp);
            });
        };

        connect_thing(ui->actionGrid, rflags::grid_outline);
        connect_thing(ui->actionHex_Coords, rflags::hex_coords);
    }

    /// Tools Panel
    {
        tools_panel = new tools_panel_t();
        tools_panel->setAllowedAreas(Qt::AllToolBarAreas);
        addToolBar(Qt::LeftToolBarArea, tools_panel);

        connect(tools_panel->actions()[tool_type_e::select],        &QAction::triggered, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::select);});
        connect(tools_panel->actions()[tool_type_e::terrain_paint], &QAction::triggered, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::terrain_paint);});
        connect(tools_panel->actions()[tool_type_e::place_objects], &QAction::triggered, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::place_objects);});
        connect(tools_panel->actions()[tool_type_e::place_splines], &QAction::triggered, [this](){ui->hexmap_widget->set_editor_tool(tool_type_e::place_splines);});
    }


    /// Terrain / Object Palette
    {
        palette_widget = new palette_widget_t();

        //palette_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        //palette_widget->setMinimumSize(200, 300);
        palette_widget->updateGeometry();

        connect(palette_widget->list_view, SIGNAL(pressed(const QModelIndex&)),
                        ui->hexmap_widget, SLOT(set_palette_item(const QModelIndex&)));

        connect(palette_widget, &palette_widget_t::terrain_add,  ui->hexmap_widget, &hexmap_widget_t::add_terrain);
        connect(palette_widget, &palette_widget_t::terrain_save, ui->hexmap_widget, &hexmap_widget_t::save_terrain);
        connect(palette_widget, &palette_widget_t::terrain_load, ui->hexmap_widget, &hexmap_widget_t::load_terrain);
    }


    //must connect initialized before handing this to the minimap_dock
    //otherwise the minimap initialize signal will have already fired before connecting
    connect(minimap, &minimap_widget_t::initialized, this, &MainWindow::minimap_initialized);
    connect(ui->hexmap_widget, &hexmap_widget_t::map_data_changed,
        [this](){
            minimap->is_dirty = true;
            minimap->update();
        });


    /// Minimap
    {
        minimap = new minimap_widget_t(*editor, this);

        connect(ui->hexmap_widget, &hexmap_widget_t::map_data_changed,
            [this](){
                minimap->is_dirty = true;
                minimap->update();
            });

        terrain_palette_model = new palette_item_model_t();
        objects_palette_model = new palette_item_model_t();

        terrain_palette_model->build_from_terrain_bank(editor->map_data.terrain_bank);
        objects_palette_model->build_from_atlas(*(editor->map_data.objects_atlas.get()));

        //lazy rebuild
        connect(ui->hexmap_widget, &hexmap_widget_t::terrain_added, palette_widget, &palette_widget_t::build_from_terrain_bank);

        editor_tool_changed(editor->current_tool);
    }


    ///Tool Settings
    {
        /// brush settings
        brush_settings = new terrain_brush_selector_t();
        //brush_settings->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        connect(brush_settings, &terrain_brush_selector_t::custom_brush_changed, this, &MainWindow::custom_terrain_brush_changed);

        /// Snap Settings
        snap_point_settings = new snap_points_widget_t(*editor);
        //snap_point_settings->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    }

    /// Object Properties
    {
        object_properties = new object_properties_widget_t();
        connect(ui->hexmap_widget, &hexmap_widget_t::object_selection_changed, this, &MainWindow::object_selection_changed);
        connect(object_properties, &object_properties_widget_t::objects_modified,
                [this]() {
                    ui->hexmap_widget->update(); //repaint
                });

        object_properties->setMinimumWidth(brush_settings->width());
    }


    /// Spline Settings
    {
        spline_settings_widget = new spline_settings_widget_t();
        spline_settings_widget->ui->LineThicknessSpinner->setValue(editor->new_node_style.thickness);


        connect(spline_settings_widget->ui->InterpolationDropDown, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
            , [this](int index)
              {
                  ASSERT(index >= 0, "");
                  auto interp = (spline_t::interpolation_e)(index);
                  ui->hexmap_widget->editor->set_current_spline_interpolation(interp);
              });


        connect(spline_settings_widget->ui->LineThicknessSpinner, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [this](double value)
                {
                    auto& editor = ui->hexmap_widget->editor;
                    auto node_style = editor->new_node_style;
                    ASSERT(value <= std::numeric_limits<float>::max(), "spline thickness too large to store in a float");
                    node_style.thickness = static_cast<float>(value);
                    ui->hexmap_widget->editor->set_spline_node_style(node_style);
                });

        connect(spline_settings_widget, &spline_settings_widget_t::colorSelected,
                [this](QColor c) {
                    auto& editor = ui->hexmap_widget->editor;
                    auto node_style = editor->new_node_style;
                    node_style.color = color_t(c.redF(), c.greenF(), c.blueF(), c.alphaF());
                    ui->hexmap_widget->editor->set_spline_node_style(node_style);
                });
    }


    /// Dock Stuff
    {
        /// Left Dock
        //tools_dock->setWidget(tools_panel);
        //tools_dock->setMaximumWidth(tools_panel->width());
        //addDockWidget(Qt::LeftDockWidgetArea, tools_dock);


        /// Right Dock
        minimap->setMinimumSize(minimap->sizeHint());
        minimap->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        minimap_dock->setFeatures(QDockWidget::DockWidgetClosable); //dont allow DockWidgetFloatable or DockWidgetMovable or else it'll break the GL context for the minimap widget when it's moved
        minimap_dock->setAllowedAreas(Qt::RightDockWidgetArea);
        minimap_dock->setWidget(minimap);
        addDockWidget(Qt::RightDockWidgetArea, minimap_dock);


        tool_settings_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        tool_settings_dock->setWidget(brush_settings);
        addDockWidget(Qt::RightDockWidgetArea, tool_settings_dock);

        palette_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        palette_dock->setWidget(palette_widget);
        addDockWidget(Qt::RightDockWidgetArea, palette_dock,       Qt::Vertical);


        ui->menuView->addSeparator();
        //ui->menuView->addAction(tools_dock->toggleViewAction());
        ui->menuView->addAction(minimap_dock->toggleViewAction());
        ui->menuView->addAction(tool_settings_dock->toggleViewAction());
        ui->menuView->addAction(palette_dock->toggleViewAction());
    }


    /// Init Hex Map
    {
        asdf::hexmap::data::hex_grid_cell_t cell;
        cell.tile_id = 0;
        glm::uvec2 map_size(default_map_width, default_map_height);
        ui->hexmap_widget->editor->new_map_action(default_map_name, map_size, cell);
        ui->hexmap_widget->zoom_extents();

        ui->hexmap_widget->set_editor_tool(tool_type_e::terrain_paint);
    }
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
    vec2 map_size_units = hxm_wgt->map_size_units();

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

    scroll_min -= scroll_padding_units;
    scroll_max += scroll_padding_units;

    auto* h_scr = ui->hexmap_hscroll;
    auto prev = h_scr->blockSignals(true);
    h_scr->setMinimum(scroll_min.x * scroll_sub_ticks);
    h_scr->setMaximum(scroll_max.x * scroll_sub_ticks);
    h_scr->setPageStep(page_step.x * scroll_sub_ticks);
    h_scr->blockSignals(prev);

    auto* v_scr = ui->hexmap_vscroll;
    prev = v_scr->blockSignals(true);
    v_scr->setMinimum(scroll_min.y * scroll_sub_ticks);
    v_scr->setMaximum(scroll_max.y * scroll_sub_ticks);
    v_scr->setPageStep(page_step.y * scroll_sub_ticks);
    v_scr->blockSignals(prev);

    scrollbar_changed();
}


void MainWindow::scrollbar_changed()
{
    glm::vec2 p(ui->hexmap_hscroll->value());
    auto scroll_range = ui->hexmap_vscroll->maximum() - ui->hexmap_vscroll->minimum();
    auto docheight = scroll_range + ui->hexmap_vscroll->pageStep();
    p.y = docheight - ui->hexmap_vscroll->value(); //since a higher camera pos moves up, but larger scrollbar value should go downward

    p /= scroll_sub_ticks;
    p.x -= asdf::hexmap::hex_width_d2; //since 0,0 is the center of a hex
    p.y -= asdf::hexmap::hex_height;

    p.y -= scroll_padding_units;

    ui->hexmap_widget->camera_pos(p, false);
    ui->hexmap_widget->update();
}

void MainWindow::new_map()
{
    ASSERT(editor, "Can't create a new map before the editor object is created");

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
    nm.set_base_tiles(editor->map_data.terrain_bank);

    if(!nm.exec()) { //exec() blocks the mainw window until the modal dialog is dismissed
        return;
    } else {
        std::string name(nm.ui->txt_mapName->displayText().toUtf8().constData());
        glm::uvec2 map_size(nm.ui->spb_width->value(), nm.ui->spb_height->value());

        asdf::hexmap::data::hex_grid_cell_t cell;
        cell.tile_id = nm.selected_base_tile_index();

        ui->hexmap_widget->editor->new_map_action(name, map_size, cell);
        ui->hexmap_widget->zoom_extents();
    }
}

QDir MainWindow::get_initial_save_load_dir()
{
    if(editor->map_filepath.size() > 0)
        return QDir(QString::fromStdString(editor->map_filepath));
    else if(editor->workspace.recently_opened.size() > 0)
        return QDir(QString::fromStdString(editor->workspace.recently_opened.back().string()));
    else
        return QDir::homePath();
}

void MainWindow::open_map()
{
    QDir initial_filepath = get_initial_save_load_dir();

    QString filepath = QFileDialog::getOpenFileName(this,
        tr("Open Map"), initial_filepath.path(), tr("Hexmap Files (*.hxm)"));

    if(filepath.size() > 0)
    {
        _open_map(filepath.toStdString());
    }
}

void MainWindow::_open_map(std::string const& filepath)
{
    ui->hexmap_widget->makeCurrent();
    editor->load_action(filepath);
    ui->hexmap_widget->zoom_extents();

    //lazy rebuild
    palette_widget->build_from_terrain_bank(hexmap_widget->editor->map_data.terrain_bank);

    set_recent_documents(editor->workspace.recently_opened);
    save_workspace();
}

void MainWindow::save_map()
{
    if(ui->hexmap_widget->editor->map_filepath.size() == 0)
    {
        save_map_as();
    }
    else
    {
        ui->hexmap_widget->editor->save_action();
        save_status_message();
    }
}

void MainWindow::save_map_as()
{
    QString dir = get_initial_save_load_dir().path();

    if(!dir.endsWith(".hxm"))
    {
        dir += "/";
        dir += QString::fromStdString(editor->map_data.map_name + "." + default_map_file_extension);
    }


    QFileDialog save_dialog(this, tr("Save Map"), dir, tr("Hexmap Files (*.hxm)"));
    save_dialog.setAcceptMode(QFileDialog::AcceptSave);
    save_dialog.setDefaultSuffix(default_map_file_extension);

    if(save_dialog.exec())
    {
        auto filenames = save_dialog.selectedFiles();
        ASSERT(filenames.size() <= 1, "There should not be multiple save filenames");
        if(filenames.size() > 0)
        {
            auto const& filepath = filenames[0];
            editor->save_action( std::string(filepath.toUtf8().constData()) );
            save_status_message();
            set_recent_documents(editor->workspace.recently_opened);
            save_workspace();
        }
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
    Q_UNUSED(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void MainWindow::set_recent_documents(vector<stdfs::path> const& recent_doc_paths)
{
    ui->menuOpen_Recent->clear();

    for(auto const& p : reversed(recent_doc_paths))
    {
        ui->menuOpen_Recent->addAction(QString::fromStdString(p.filename().string())
          , [this, p]()
            {
                if(stdfs::is_regular_file(p))
                {
                    _open_map(p.string());
                }
                else
                {
                    auto& recents = editor->workspace.recently_opened;
                    recents.erase(std::find(recents.begin(), recents.end(), p));
                    QMessageBox::about(this, "File Not Found"
                        , "This file has been deleted or moved. It will be removed from the 'Open Recent' list");
                }
            }
        );
    }

    if(recent_doc_paths.size() > 0)
    {
        ui->menuOpen_Recent->addSeparator();
        ui->menuOpen_Recent->addAction("Clear Recents"
          , [this]()
            {
                editor->workspace.recently_opened.clear();
                set_recent_documents(vector<stdfs::path>());
                save_workspace();
            }
        );
    }
}

void MainWindow::save_status_message()
{
    QString save_str = "Map Saved to \"";
    save_str.append(ui->hexmap_widget->editor->map_filepath.c_str());
    statusBar()->showMessage(save_str, status_message_timeout_ms);
}

std::string MainWindow::workspace_path() const
{
    /// In the future, I'll use the native settings path
    /// for now, just use the current working directory
    /// (which should be the same dir as the executable)
    //auto appdata_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    //auto workspace_path = appdata_path + "/" + editor_workspace_filename;
    //std::string workspace_path_str = workspace_path.toStdString();

    return std::string(editor_workspace_filename);
}

void MainWindow::load_workspace_create_if_not_exists()
{
    auto path_str = workspace_path();

    if(stdfs::exists(stdfs::path(path_str)))
    {
        editor->load_workspace(path_str);
    }
    else
    {
        editor->save_workspace(path_str);
    }
}

void MainWindow::save_workspace()
{
    editor->save_workspace(workspace_path());
}


void MainWindow::minimap_initialized()
{
    /// fixme??
    //minimap->map_data.terrain_bank = ui->hexmap_widget->editor->rendered_map->terrain_bank;
}

void MainWindow::editor_tool_changed(tool_type_e new_tool)
{
    switch(new_tool)
    {
        case tool_type_e::select:
            palette_dock->setWidget(object_properties);

            tool_settings_dock->setWidget(nullptr);
            break;

        case tool_type_e::terrain_paint:
            palette_widget->list_view->setModel(terrain_palette_model);
            palette_dock->setWidget(palette_widget);
            palette_dock->setWindowTitle(tr("Terrain"));

            tool_settings_dock->setWidget(brush_settings);
            break;

        case tool_type_e::place_objects:
            palette_widget->list_view->setModel(objects_palette_model);
            palette_dock->setWidget(palette_widget);
            palette_dock->setWindowTitle(tr("Objects"));

            tool_settings_dock->setWidget(snap_point_settings);
            break;

        case tool_type_e::place_splines:
            palette_dock->setWidget(spline_settings_widget);

            tool_settings_dock->setWidget(snap_point_settings);
            break;

        default:
            palette_dock->setWidget(nullptr);
            tool_settings_dock->setWidget(nullptr);
            //palette_widget->list_view->setModel(nullptr);
            break;
    }
}

void MainWindow::object_selection_changed(asdf::hexmap::editor::editor_t& editor)
{
    object_properties->set_from_object_selection(editor.object_selection, editor.map_data.objects);
}

void MainWindow::custom_terrain_brush_changed(asdf::hexmap::data::terrain_brush_t const& brush)
{
    editor->set_custom_terrain_brush(brush);
}
