#include "minimap_widget.h"

#include <asdf_multiplat/main/asdf_multiplat.h>

using namespace asdf;

minimap_widget_t::minimap_widget_t(asdf::hexmap::editor::editor_t& _edtior, QWidget *parent) :
    QOpenGLWidget(parent)
  , map_data(_edtior.map_data)
{
}

minimap_widget_t::~minimap_widget_t()
{
}

void minimap_widget_t::initializeGL()
{
    using namespace hexmap;

    ASSERT(asdf::app.renderer, "asdf app renderer should exist by now");

    rendered_map = std::make_unique<ui::hex_map_t>(map_data);
    minimap = std::make_unique<ui::minimap_t>(*(rendered_map.get()));
}

void minimap_widget_t::resizeGL(int w, int h){
    ASSERT(minimap, "");
    minimap->render_target.texture.write(nullptr, w, h); //resize render target texture
    is_dirty = true;
}

void minimap_widget_t::paintGL(){
    //ASSERT(!CheckGLError(), "GL Error");
    CheckGLError(); //clear gl errors. I think QT is doing something stupid
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


    if(is_dirty)
    {
        minimap->rebuild();
    }

    //renders the prebuilt minimap texture onto a quad
    minimap->render();
}

void minimap_widget_t::mousePressEvent(QMouseEvent*){
}

void minimap_widget_t::mouseReleaseEvent(QMouseEvent*){
}

void minimap_widget_t::mouseMoveEvent(QMouseEvent*){
}

void minimap_widget_t::wheelEvent(QWheelEvent*){
}
