#ifndef HEXMAP_WIDGET_T_H
#define HEXMAP_WIDGET_T_H

#include <hexmap/ui/hex_map.h>  //put before any QT stuff that will include openGL

#include <QOpenGLWidget>

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

protected:
    asdf::hexmap::data::hex_grid_t hex_grid;
    std::unique_ptr<asdf::hexmap::ui::hex_map_t> hex_map;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

#endif // HEXMAP_WIDGET_T_H
