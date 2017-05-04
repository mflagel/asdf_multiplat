#include "minimap_widget.h"

#include <asdf_multiplat/main/asdf_multiplat.h>
#include <glm/gtc/matrix_transform.hpp>

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
    using namespace asdf;
    using namespace hexmap;

    ASSERT(app.renderer, "asdf app renderer should exist by now");

    //apparently this func gets called again when the widget is in a dock widget that gets undocked
    if(!gl_state)
    {
        void* qt_gl_context = reinterpret_cast<void*>(context());
        gl_state = std::make_unique<gl_state_t>(qt_gl_context);
        GL_State.set_current_state_machine(*(gl_state.get()));

        rendered_map = std::make_unique<ui::hex_map_t>(map_data);
        minimap = std::make_unique<ui::minimap_t>(*(rendered_map.get()));
    }
}

void minimap_widget_t::resizeGL(int w, int h){
    ASSERT(minimap, "");
    minimap->render_target.texture.write(nullptr, w, h); //resize render target texture

    auto& camera = rendered_map->camera;
    camera.set_aspect_ratio(w, h);
    camera.viewport = asdf::viewport_for_size_aspect(map_data.hex_grid.size_units(), camera.aspect_ratio);

    is_dirty = true;
}

void minimap_widget_t::paintGL(){
    //ASSERT(!CheckGLError(), "GL Error");
    CheckGLError(); //clear gl errors. I think QT is doing something stupid

    //set global GL_State proxy object to point to this context
    ASSERT(reinterpret_cast<void*>(context()) == gl_state->gl_context, "wtf? the GL context changed");
    GL_State.set_current_state_machine(*(gl_state.get()));

    //put the widget's fbo at the bottom of the stack
    auto const& t = minimap->render_target.texture;
    scoped_fbo_t scoped(defaultFramebufferObject(), 0,0, t.width, t.height);

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
        is_dirty = false;
    }



    using namespace asdf;
    using namespace glm;

    //renders the prebuilt minimap texture onto a quad
    auto const& shader = app.renderer->screen_shader;
    GL_State->bind(shader);

    shader->world_matrix = mat4();
    shader->view_matrix = mat4();
    shader->projection_matrix = glm::ortho<float>(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
    shader->update_wvp_uniform();

    glBindTexture(GL_TEXTURE_2D, minimap->render_target.texture.texture_id);
    GL_State->bind(app.renderer->quad.vbo);
    asdf_renderer_t::quad_vertex_t::vertex_spec.set_vertex_attribs(shader);
    glDrawArrays(app.renderer->quad.draw_mode, 0, app.renderer->quad.num_verts);
    GL_State->unbind_vbo();

    GL_State->unbind_shader();
}

void minimap_widget_t::mousePressEvent(QMouseEvent*){
}

void minimap_widget_t::mouseReleaseEvent(QMouseEvent*){
}

void minimap_widget_t::mouseMoveEvent(QMouseEvent*){
}

void minimap_widget_t::wheelEvent(QWheelEvent*){
}
