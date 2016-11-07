#include "hexmap_widget.h"

#include <asdf_multiplat/main/asdf_multiplat.h>
#include <asdf_multiplat/data/content_manager.h>

#include "mainwindow.h"

/// https://doc-snapshots.qt.io/qt5-dev/qopenglwidget.html

using namespace std;
using namespace glm;

hexmap_widget_t::hexmap_widget_t(QWidget* _parent)
: QOpenGLWidget(_parent)
, data_map(ivec2(15, 16))
{

}

void hexmap_widget_t::initializeGL()
{
    // Set up the rendering context, load shaders and other resources, etc.:

    asdf::app.renderer = make_unique<asdf::asdf_renderer_t>();  //do this before content init. Content needs GL_State to exist
    asdf::app.renderer->init(); //loads screen shader, among other things
    asdf::Content.init(); //needed for Content::shader_path

    auto shader = asdf::Content.create_shader("hexmap", 330);
    asdf::Content.shaders.add_resource(shader);

    hex_map = make_unique<asdf::hexmap::ui::hex_map_t>(data_map);

    hex_map->camera.position.z = 10;
}

void hexmap_widget_t::resizeGL(int w, int h)
{
    hex_map->camera.viewport.size_d2 = vec2(w,h) / 2.0f;
    hex_map->camera.viewport.bottom_left = -1.0f * hex_map->camera.viewport.size_d2;

    ASSERT(main_window, "main_window ptr expected");
    main_window->set_scrollbar_stuff();
}

void hexmap_widget_t::paintGL()
{
    glDisable(GL_DEPTH_TEST);

    auto& gl_clear_color = asdf::app.renderer->gl_clear_color;
    glClearColor(gl_clear_color.r
                       , gl_clear_color.g
                       , gl_clear_color.b
                       , gl_clear_color.a);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE);

    hex_map->render();

}


glm::ivec2 hexmap_widget_t::map_size() const
{
    return data_map.hex_grid.size;
}

glm::vec2 hexmap_widget_t::camera_pos() const
{
    return vec2(hex_map->camera.position.x, hex_map->camera.position.y);
}

void hexmap_widget_t::camera_pos(glm::vec2 p)
{
    ASSERT(hex_map, "can't set camera info when the hex_map containing the camera doesnt exist yet");
    hex_map->camera.position.x = p.x;
    hex_map->camera.position.y = p.y;
}
