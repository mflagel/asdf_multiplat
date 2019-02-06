#include "hexmap_widget.h"

#include <QMouseEvent>
#include <QModelIndex>

#include <asdfm/main/asdf_multiplat.h>
#include <asdfm/data/content_manager.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "terrain_brush_selector.h"

/// https://doc-snapshots.qt.io/qt5-dev/qopenglwidget.html

using namespace std;
//using namespace glm;  //causes namespace collison with uint

using vec2 = glm::vec2;

namespace
{
    constexpr float zoom_per_scroll_tick = 0.5f;

    constexpr float min_zoom = -16.0f;
    constexpr float max_zoom = 128.0f;
}

using tool_type_e = asdf::hexmap::editor::editor_t::tool_type_e;

hexmap_widget_t::hexmap_widget_t(QWidget* _parent)
: QOpenGLWidget(_parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void hexmap_widget_t::initializeGL()
{
    using namespace asdf;
    
    void* qt_gl_context = reinterpret_cast<void*>(context());  //this technically isnt the native context pointer, but I don't think I care so long as it's unique
    app.renderer = make_unique<asdf::asdf_renderer_t>(qt_gl_context);  //do this before content init. Content needs GL_State to exist
    GL_State.set_current_state_machine(app.renderer->gl_state);

    app.renderer->init(); //loads screen shader, among other things
    Content.init(); //needed for Content::shader_path

    defaultFramebufferObject();

    auto shader = Content.create_shader_highest_supported("hexmap");
    Content.shaders.add_resource(shader);

    hexmap::editor::editor_t test_editor;
    test_editor.set_current_object_id(0);

    editor = std::make_unique<hexmap::editor::editor_t>();
    editor->init();
    // hex_map = &(editor.rendered_map);

    editor->map_changed_callback = [this](){
        emit map_data_changed(editor->map_data);
    };

    emit hex_map_initialized();
}

void hexmap_widget_t::resizeGL(int w, int h)
{
    using namespace asdf;

    app.surface_width = w;
    app.surface_height = h;

    editor->resize(w, h);
    main_window->set_scrollbar_stuff(editor->rendered_map.camera);
    //emit camera_changed(hex_map->camera);
}

void hexmap_widget_t::paintGL()
{
    using namespace asdf;

    //set global GL_State proxy object to point to this context
    GL_State.set_current_state_machine(app.renderer->gl_state);

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

    editor->render();
}


glm::uvec2 hexmap_widget_t::map_size_cells() const
{
    return editor->map_data.hex_grid.size_cells();
}

glm::vec2  hexmap_widget_t::map_size_units() const
{
    return editor->map_data.hex_grid.size_units();
}

glm::vec2 hexmap_widget_t::camera_pos() const
{
    return vec2(editor->rendered_map.camera.position.x, editor->rendered_map.camera.position.y);
}

void hexmap_widget_t::camera_pos(glm::vec2 const& p, bool emit_signal)
{
    editor->rendered_map.camera.position.x = p.x;
    editor->rendered_map.camera.position.y = p.y;

    //if(emit_signal)
        //emit camera_changed(hex_map->camera);
}

//final zoom is equal to 2^zoom_exponent
void hexmap_widget_t::camera_zoom_exponent(float zoom_exponent)
{
    editor->rendered_map.camera.position.z = zoom_exponent;
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

    auto obj_sel = editor->object_selection;
    mouse.mouse_up(asdf_event, adjusted_screen_coords(event->x(), event->y()));

    if(obj_sel != editor->object_selection)
    {
        emit object_selection_changed(*editor);
    }

    update();
}

void hexmap_widget_t::mouseMoveEvent(QMouseEvent* event)
{
    auto& mouse = asdf::app.mouse_state;

    asdf::mouse_motion_event_t asdf_event {
        mouse
    };

    auto coords = adjusted_screen_coords(event->x(), event->y());

    if(mouse.is_dragging_any_of())
    {
        mouse.mouse_drag(asdf_event, coords);
    }
    else
    {
        mouse.mouse_move(asdf_event, coords);
    }

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
        // because shift button is held, the hscroll will act as if I'm shift-scrolling the scrollbar
        // which uses large page-sized jumps instead of regular jumps
        Qt::KeyboardModifiers mods = event->modifiers();
        mods.setFlag(Qt::ShiftModifier, false);
        event->setModifiers(mods);
        main_window->ui->hexmap_hscroll->event(event);
    }
    else if(keyboard_mods == Qt::ControlModifier)
    {
        float num_steps = event->angleDelta().y() / 8.0f / 15.0f;

        auto& zoom = editor->rendered_map.camera.position.z;

        zoom += num_steps * zoom_per_scroll_tick;
        zoom = glm::clamp(zoom, min_zoom, max_zoom);

        main_window->set_scrollbar_stuff(editor->rendered_map.camera);
        update();
        emit camera_changed(editor->rendered_map.camera);
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

    auto prev_tool = editor->current_tool;

    editor->input->on_key_down(sdl_key_event);

    if(prev_tool != editor->current_tool)
        emit editor_tool_changed(editor->current_tool);

    /// Hxm_Qt specific hotkeys
    switch(event->key())
    {
        case Qt::Key_BracketLeft:
            main_window->brush_settings->shrink_brush();
            update();
            return;
        case Qt::Key_BracketRight:
            main_window->brush_settings->grow_brush();
            update();
            return;
    }


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
    editor->set_tool(new_tool);
    emit editor_tool_changed(new_tool);
}

void hexmap_widget_t::set_palette_item(QModelIndex const& index)
{
    switch(editor->current_tool)
    {
        case tool_type_e::terrain_paint:
            editor->set_current_tile_id(index.row());
            break;
        case tool_type_e::place_objects:
            editor->set_current_object_id(index.row());
            break;

    default:
        break;
    }
}


void hexmap_widget_t::add_terrain(QStringList const& terrain_filepaths)
{
    if(terrain_filepaths.size() > 0)
    {
        //ensure this is the current gl context before the terrain bank
        //does any rendering in add_texture
        makeCurrent();

        for(auto const& filepath : terrain_filepaths)
        {
            std::string filepath_str{filepath.toUtf8().constData()};

            editor->map_data.terrain_bank.add_texture(filepath_str);
        }

        emit terrain_added(editor->map_data.terrain_bank);
    }
}

void hexmap_widget_t::save_terrain(QString const& filepath)
{
    auto path = std::experimental::filesystem::path(filepath.toUtf8().constData());
    editor->map_data.terrain_bank.save_to_file(path);
}

void hexmap_widget_t::load_terrain(QString const& filepath)
{
    makeCurrent();
    auto path = std::experimental::filesystem::path(filepath.toUtf8().constData());

    editor->map_data.terrain_bank.clear();
    editor->map_data.terrain_bank.load_from_file(path);

    emit terrain_added(editor->map_data.terrain_bank);
}

void hexmap_widget_t::zoom_to_selection()
{

}

void hexmap_widget_t::zoom_extents()
{
    camera_pos(glm::vec2(editor->map_data.hex_grid.size) / 2.0f);
    main_window->set_scrollbar_stuff(editor->rendered_map.camera);
}
