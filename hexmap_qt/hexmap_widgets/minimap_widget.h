#ifndef MINIMAP_WIDGET_H
#define MINIMAP_WIDGET_H

#include <hexmap/editor/main/editor.h>

#include <QOpenGLWidget>

namespace Ui {
class minimap_widget_t;
}

class hexmap_widget_t;


/*
Because this is a seperate QOpenGLWidget, an entirely separate openGL context is created
This makes resource sharing kind of weird.

https://blog.gvnott.com/some-usefull-facts-about-multipul-opengl-contexts/
In contrast ‘State’ objects are not shared between contexts, including but not limited to:
Vertex Array Objects (VAOs)
Framebuffer Objects (FBOs)

Because of this, I'm creating an entirely seperate rendered_map object
rather than sharing the one from editor_t contained in the hexmap_widget
*/
class minimap_widget_t : public QOpenGLWidget
{
    Q_OBJECT

public:
    asdf::hexmap::data::hex_map_t& map_data;
    std::unique_ptr<asdf::hexmap::ui::hex_map_t> rendered_map; //probably won't need this when hexmap::minimap_t has a better rendering method
    std::unique_ptr<asdf::hexmap::ui::minimap_t> minimap;

    bool is_dirty = false;

    explicit minimap_widget_t(asdf::hexmap::editor::editor_t&, QWidget *parent = 0);
    ~minimap_widget_t();

    QSize sizeHint() const override;

signals:
    void initialized();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

private:
    hexmap_widget_t* hexmap_widget = nullptr;
    std::unique_ptr<asdf::gl_state_t> gl_state;
    Ui::minimap_widget_t *ui;
};

#endif // MINIMAP_WIDGET_H
