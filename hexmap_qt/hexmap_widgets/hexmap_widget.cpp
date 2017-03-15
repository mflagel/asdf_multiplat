#include "hexmap_widget.h"

#include <QMouseEvent>

#include <asdf_multiplat/main/asdf_multiplat.h>
#include <asdf_multiplat/data/content_manager.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

/// https://doc-snapshots.qt.io/qt5-dev/qopenglwidget.html

using namespace std;
//using namespace glm;  //causes namespace collison with uint

using vec2 = glm::vec2;

namespace
{
    constexpr float zoom_per_scroll_tick = 0.1f;
}

using tool_type_e = asdf::hexmap::editor::editor_t::tool_type_e;

hexmap_widget_t::hexmap_widget_t(QWidget* _parent)
: QOpenGLWidget(_parent)
, data_map(editor.map_data)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void hexmap_widget_t::initializeGL()
{
    // Set up the rendering context, load shaders and other resources, etc.:

    asdf::app.renderer = make_unique<asdf::asdf_renderer_t>();  //do this before content init. Content needs GL_State to exist
    asdf::app.renderer->init(); //loads screen shader, among other things
    asdf::Content.init(); //needed for Content::shader_path

    auto shader = asdf::Content.create_shader_highest_supported("hexmap");
    asdf::Content.shaders.add_resource(shader);

    editor.init();
    hex_map = editor.rendered_map.get();

    hex_map->camera.position.z = 10;

    hex_map_initialized(editor);
}

void hexmap_widget_t::resizeGL(int w, int h)
{
    hex_map->camera.viewport.size_d2 = vec2(w,h) / 2.0f;
    hex_map->camera.viewport.bottom_left = -1.0f * hex_map->camera.viewport.size_d2;

    emit camera_changed(hex_map->camera);
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

    editor.render();
}


glm::uvec2 hexmap_widget_t::map_size() const
{
    return data_map.hex_grid.size;
}

glm::vec2 hexmap_widget_t::camera_pos() const
{
    return vec2(hex_map->camera.position.x, hex_map->camera.position.y);
}

void hexmap_widget_t::camera_pos(glm::vec2 const& p, bool emit_signal)
{
    hex_map->camera.position.x = p.x;
    hex_map->camera.position.y = p.y;

    if(emit_signal)
        emit camera_changed(hex_map->camera);
}


// there may be a better solution than a switch statement
asdf::mouse_button_e asdf_button_from_qt(Qt::MouseButton button)
{
    switch(button)
    {
        case Qt::NoButton:
            return asdf::mouse_no_button;
        case Qt::LeftButton:
            return asdf::mouse_left;
        case Qt::RightButton:
            return asdf::mouse_right;
        case Qt::MiddleButton:
            return asdf::mouse_middle;
        case Qt::ExtraButton1:
            return asdf::mouse_4;
        case Qt::ExtraButton2:
            return asdf::mouse_5;

        default:
            return asdf::mouse_no_button;
    }
}


void hexmap_widget_t::mousePressEvent(QMouseEvent* event)
{
    auto& mouse = asdf::app.mouse_state;

    asdf::mouse_button_event_t asdf_event {
      mouse
    , asdf_button_from_qt(event->button())
    , (event->flags() & Qt::MouseEventCreatedDoubleClick) > 0
    };

    LOG("x,y: %i, %i", event->x(), event->y());

    mouse.mouse_down(asdf_event, adjusted_screen_coords(event->x(), event->y()));
    update();
}

void hexmap_widget_t::mouseReleaseEvent(QMouseEvent* event)
{
    auto& mouse = asdf::app.mouse_state;

    asdf::mouse_button_event_t asdf_event {
      mouse
    , asdf_button_from_qt(event->button())
    , (event->flags() & Qt::MouseEventCreatedDoubleClick) > 0
    };

    mouse.mouse_up(asdf_event, adjusted_screen_coords(event->x(), event->y()));
    update();
}

void hexmap_widget_t::mouseMoveEvent(QMouseEvent* event)
{
    auto& mouse = asdf::app.mouse_state;

    asdf::mouse_motion_event_t asdf_event {
        mouse
    };

    mouse.mouse_move(asdf_event, adjusted_screen_coords(event->x(), event->y()));
    update(); //lazy
}

void hexmap_widget_t::wheelEvent(QWheelEvent* event)
{
    if(keyboard_mods == Qt::NoModifier)
    {
        main_window->ui->hexmap_vscroll->event(event); ///FIXME can this be solved without holding onto main_window?
    }
    else if(keyboard_mods == Qt::ShiftModifier)
    {
        main_window->ui->hexmap_hscroll->event(event);
    }
    else if(keyboard_mods == Qt::ControlModifier)
    {
        float num_steps = event->angleDelta().y() / 15.0f;
        hex_map->camera.position.z += num_steps * zoom_per_scroll_tick;
        update();
        emit camera_changed(hex_map->camera);
    }
}


void hexmap_widget_t::keyPressEvent(QKeyEvent* event)
{
    keyboard_mods = event->modifiers();

    SDL_Keysym sdl_key_event; //being lazy and reusing sdl event for now
    sdl_key_event.sym = event->nativeVirtualKey(); //supposedly virtual keys are a standard

    sdl_key_event.mod = 0;
    sdl_key_event.mod |= KMOD_SHIFT * (event->modifiers() & Qt::ShiftModifier) > 0;
    sdl_key_event.mod |= KMOD_CTRL  * (event->modifiers() & Qt::ControlModifier) > 0;
    sdl_key_event.mod |= KMOD_ALT   * (event->modifiers() & Qt::AltModifier) > 0;
    sdl_key_event.mod |= KMOD_GUI   * (event->modifiers() & Qt::MetaModifier) > 0;

    auto prev_tool = editor.current_tool;

    editor.input->on_key_down(sdl_key_event);

    if(prev_tool != editor.current_tool)
        emit editor_tool_changed(editor.current_tool);

    /// TODO only call this when editor does not handle key
    QWidget::keyPressEvent(event);
}

void hexmap_widget_t::keyReleaseEvent(QKeyEvent *event)
{
    keyboard_mods = event->modifiers();
}



// QT mouse coords have 0,0 at the top-left of the widget
// adjust such that 0,0 is the center
glm::ivec2 hexmap_widget_t::adjusted_screen_coords(int x, int y) const
{
    return glm::ivec2(x - width()/2, height()/2 - y);
}

void hexmap_widget_t::set_editor_tool(tool_type_e new_tool)
{
    editor.set_tool(new_tool);
    emit editor_tool_changed(new_tool);
}

void hexmap_widget_t::set_palette_item(QModelIndex const& index)
{
    switch(editor.current_tool)
    {
        case tool_type_e::terrain_paint:
            editor.current_tile_id = index.row();
            break;
        case tool_type_e::place_objects:
            editor.current_object_id = index.row();
            break;

    default:
        break;
    }
}
