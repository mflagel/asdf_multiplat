#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <hexmap/ui/hex_map.h>  //put before any QT stuff that will include openGL

#include <QMainWindow>
#include <QDir>

#include "hexmap/editor/main/editor.h"

namespace Ui {
class MainWindow;
}

class palette_widget_t;
class palette_item_model_t;
class spline_settings_widget_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class hexmap_widget_t;

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

protected:
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private slots:
    void hex_map_initialized(asdf::hexmap::editor::editor_t&);
    void editor_tool_changed(asdf::hexmap::editor::editor_t::tool_type_e);

private:
    Ui::MainWindow *ui;

    asdf::hexmap::editor::editor_t* editor = nullptr;

    palette_widget_t* palette_widget = nullptr;
    palette_item_model_t* terrain_palette_model = nullptr;
    palette_item_model_t* objects_palette_model = nullptr;

    spline_settings_widget_t* spline_settings_widget = nullptr;

    glm::vec2 base_camera_offset;

    QDir current_save_dir;
};

#endif // MAINWINDOW_H
