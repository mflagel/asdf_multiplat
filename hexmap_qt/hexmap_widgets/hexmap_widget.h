#ifndef HEXMAP_WIDGET_T_H
#define HEXMAP_WIDGET_T_H

#include <hexmap/ui/hex_map.h>  //put before any QT stuff that will include openGL

#include <QOpenGLWidget>

#include <hexmap/editor/main/editor.h>

class MainWindow;

class hexmap_widget_t : public QOpenGLWidget
{
public:
    hexmap_widget_t(QWidget* parent);

    glm::ivec2 map_size() const;

    glm::vec2 camera_pos() const;
    void camera_pos(glm::vec2);

    float camera_zoom() const { return hex_map->camera.zoom(); }

    MainWindow* main_window = nullptr;

    //encapsulation is obnoxious
    asdf::hexmap::ui::hex_map_t* hex_map;
    asdf::hexmap::data::hex_map_t& data_map;

protected:
    asdf::hexmap::editor::editor_t editor;


    Qt::KeyboardModifiers keyboard_mods;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    glm::ivec2 adjusted_screen_coords(int x, int y) const; //not 100% happy with this name
};

#endif // HEXMAP_WIDGET_T_H
