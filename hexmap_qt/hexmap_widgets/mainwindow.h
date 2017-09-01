#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <hexmap/ui/hex_map.h>  //put before any QT stuff that will include openGL

#include <QMainWindow>
#include <QDir>

#include "hexmap/editor/main/editor.h"

namespace Ui {
class MainWindow;
}

class QSpinBox;

class palette_widget_t;
class palette_item_model_t;
class spline_settings_widget_t;
class object_properties_widget_t;
class minimap_widget_t;
class terrain_brush_selector_t;
class hexmap_widget_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class hexmap_widget_t;
    friend class minimap_widget_t;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void set_scrollbar_stuff(asdf::camera_t const&);


public slots:
    void scrollbar_changed();

    void new_map();
    void open_map();
    void save_map();
    void save_map_as();

    void undo();
    void redo();

protected:
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private:
    void save_status_message();

private slots:
    void init();
    //void hex_map_initialized(asdf::hexmap::editor::editor_t&);
    void minimap_initialized();
    void editor_tool_changed(asdf::hexmap::editor::editor_t::tool_type_e);
    void object_selection_changed(asdf::hexmap::editor::editor_t&);
    void custom_terrain_brush_changed(asdf::hexmap::data::terrain_brush_t const&);

private:
    Ui::MainWindow *ui;

    QSpinBox* zoom_spinbox = nullptr;

    asdf::hexmap::editor::editor_t* editor = nullptr;

    hexmap_widget_t* hexmap_widget = nullptr;
    QDockWidget* palette_dock = nullptr;
    QDockWidget* minimap_dock = nullptr;

    palette_widget_t* palette_widget = nullptr;
    palette_item_model_t* terrain_palette_model = nullptr;
    palette_item_model_t* objects_palette_model = nullptr;

    minimap_widget_t* minimap = nullptr;
    object_properties_widget_t* object_properties = nullptr;
    terrain_brush_selector_t* brush_settings = nullptr;

    spline_settings_widget_t* spline_settings_widget = nullptr;

    QDir current_save_dir;
};

#endif // MAINWINDOW_H
