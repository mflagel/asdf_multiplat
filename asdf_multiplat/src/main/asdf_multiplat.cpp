#include "stdafx.h"
#include "asdf_multiplat.h"

#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/gtx/rotate_vector.hpp>

#include "main/input_sdl.h"
#include "data/gl_resources.h"
#include "data/content_manager.h"
//#include "ui/ui_base.h"
#include "utilities/spritebatch.h"
#include "utilities/utilities.h"


using namespace asdf::util;
using namespace std;
using namespace glm;

namespace asdf {

    ASDFM_API asdf_multiplat_t app;

    asdf_multiplat_t::asdf_multiplat_t()
        : WINDOW_TITLE("Asdf Multiplat Application Thing")
    {
        std::set_terminate(util::terminate_handler);
        signal(SIGINT,util::interrupt_handler);
    }

    asdf_multiplat_t::~asdf_multiplat_t()
    {
        SDL_FreeSurface(main_surface);
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(main_window);
        SDL_Quit();
    }

    void asdf_multiplat_t::init(std::string _exec_dir) {
        LOG("--- Initializing This Crazy Contraption ---");

        running = true;

        exec_dir = _exec_dir;
        working_directory = get_current_working_directory();
        SetCurrentDir(exec_dir.c_str());

        LOG("Exec Dir: %s", exec_dir.c_str());
        LOG("Working Dir: %s", working_directory.c_str());

        init_SDL();
        in_focus = true;

        renderer = make_unique<asdf_renderer_t>(gl_context);
        GL_State.current_state_machine = &(renderer->gl_state);
        renderer->init();

        Content.init();
        spritebatch = make_shared<spritebatch_t>();
        spritebatch->spritebatch_shader = Content.shaders["spritebatch"];
    }

    void asdf_multiplat_t::resize(uint32_t w, uint32_t h)
    {
        ASSERT(settings.resizable, "Why is a window being resized when settings say it can't be");

        surface_width = w;
        surface_height = h;

        //for now the rendered resolution will always match the surface size
        settings.resolution_width = w;
        settings.resolution_width = h;

        renderer->resize(w, h);
        specific->resize(w, h);
    }

    void asdf_multiplat_t::update() {
        uint32_t currentTicks = SDL_GetTicks();
        float timeElapsed = (currentTicks - prev_ticks) / 1000.0f;

        frame_times[frame_time_index++] = timeElapsed;
        frame_time_index %= frame_times.size();

        prev_ticks = currentTicks;

        if (!in_focus)
            return;

        ASSERT(specific, "app.specific not assigned");
        specific->update(timeElapsed);

        //main_view->update(timeElapsed);

        //mouse_state.update();
        //keyboardState->Update();

        //gameScreen->Update(timeElapsed, *mouseState, *keyboardState);

        //keyboardState->UpdatePrevState();
        //mouseState->UpdatePreviousState();
    }

    float asdf_multiplat_t::average_frame_time() const
    {
        float sum = 0;
        for(auto const& t : frame_times)
            sum += t;

        return sum / static_cast<float>(frame_times.size());
    }

    void asdf_multiplat_t::render() {
        renderer->pre_render();

        {
            // spritebatch->begin();
            // ASSERT(!CheckGLError(), "gl error after begin");
            // main_view->render(glm::vec3(0, 0, 0), glm::mat3(), color_t(1.0f));
            // ASSERT(!CheckGLError(), "GL error after main_view->render() ");
            // spritebatch->end();
        }

        glUseProgram(0);

        if (render_debug_views)
            render_debug();

        ASSERT(specific, "app.specific not assigned");
        specific->render();

        renderer->post_render();
    }

    void asdf_multiplat_t::render_debug() {
        
        LOG_IF(CheckGLError(), "Error before drawing spritebatch debug information");

        auto passthrough = Content.shaders["passthrough"];
        passthrough->use_program();
        passthrough->world_matrix = glm::mat4();
        passthrough->view_matrix = glm::mat4();
        passthrough->projection_matrix = spritebatch->spritebatch_shader->projection_matrix;

        passthrough->update_wvp_uniform();
        glUniform4f(passthrough->uniforms["Color"], 0.0f, 0.2f, 1.0f, 1.0f);

        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.2f);

        //glBegin(GL_POLYGON);
        //glColor4f(0.5f, 0.3f, 1.0f, 1.0f);
        //glVertex2f(-1, 1);
        //glVertex2f(1, 1);
        //glVertex2f(1, -1);
        //glVertex2f(-1, -1);
        //glEnd();

        //glBegin(GL_POLYGON);
        //glColor4f(0.3f, 0.1f, 0.3f, 1.0f);
        //glVertex2f(-0.5f, 0.5f);
        //glVertex2f(0.5f, 0.5f);
        //glVertex2f(0.5f, -0.5f);
        //glVertex2f(-0.5f, -0.5f);
        //glEnd();

        //main_view->render_debug();

        glUseProgram(0);
    }

    void asdf_multiplat_t::on_event(SDL_Event* event) {
        switch (event->type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                if (event->key.keysym.sym == SDLK_F4) {
                    if (event->key.keysym.mod & KMOD_ALT)
                        running = false;
                }
                else if (event->key.keysym.sym == SDLK_F12)
                {
                    save_screenshot("screenshot_test.bmp");
                }
                break;

            case SDL_MOUSEMOTION:
                event->motion.x -= unsigned_to_signed(surface_width)  / 2;
                event->motion.y =  unsigned_to_signed(surface_height) / 2 - event->motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                event->button.x -= unsigned_to_signed(surface_width) / 2;
                event->motion.y  = unsigned_to_signed(surface_height) / 2 - event->motion.y;
                break;

            case SDL_MOUSEWHEEL:
                break;

            case SDL_WINDOWEVENT:
            {
                switch(event->window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:// [[FALLTHROUGH]]
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    {
                        ASSERT(event->window.data1 >= 0 && event->window.data2 >= 0, "");
                        resize(uint32_t(event->window.data1), uint32_t(event->window.data2));
                        break;
                    }
                };

                break;
            }
        }


        process_sdl_mouse_events(mouse_state, event);
        specific->on_event(event);
        //main_view->on_event(event, glm::vec3(), glm::mat3());
    }

    void asdf_multiplat_t::init_SDL() {
        //init video subsystem
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            EXPLODE("Could not initialize SDL: %s.", SDL_GetError());
            running = false;
            return;
        }

        //set GL version to 3.3
        //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); //set 24 bit depth buffer
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //turn on double buffering

        //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


        //create window
        uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        flags |= SDL_WINDOW_FULLSCREEN * settings.fullscreen;
        flags |= SDL_WINDOW_BORDERLESS * settings.borderless;
        flags |= SDL_WINDOW_RESIZABLE  * settings.resizable;

        main_window = SDL_CreateWindow(WINDOW_TITLE.c_str()
                                     , SDL_WINDOWPOS_CENTERED
                                     , SDL_WINDOWPOS_CENTERED
                                     , unsigned_to_signed(settings.resolution_width)
                                     , unsigned_to_signed(settings.resolution_height)
                                     , flags
                                     );
        checkSDLError(__LINE__);
        ASSERT(main_window != 0, "Unable to create window");

        surface_width = settings.resolution_width;
        surface_height = settings.resolution_height;

        //create OpenGL context
        gl_context = SDL_GL_CreateContext(main_window);
        checkSDLError(__LINE__);

        ASSERT(gl_context != nullptr, "Unable to create OpenGL Context");

        prev_ticks = SDL_GetTicks();
        //SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

        SDL_GL_SetSwapInterval(1);
        LOG("SDL Initialized");
        LOG("Window Size: %d x %d", surface_width, surface_height);
    }

    void asdf_multiplat_t::make_GL_current()
    {
        SDL_GL_MakeCurrent(main_window, gl_context);
    }


    //todo: append date/time info so I get unique screenshot names
    //todo: save as png
    void asdf_multiplat_t::save_screenshot(std::string file_path) const
    {
        // if the last 3 characters aren't "bmp", tack on ".bmp"
        // check the length first, rather than trying to compare with a substring of size - 3 where size might be smaller than 3
        if (file_path.size() > 4 && (file_path.compare(file_path.size() - 3, 3, "bmp") != 0))
        {
            file_path += ".bmp";
        }

        auto save_result = SOIL_save_screenshot
        (
            file_path.c_str()
          , SOIL_SAVE_TYPE_BMP
          , 0, 0
          , unsigned_to_signed(surface_width)
          , unsigned_to_signed(surface_height)
        );

        //soil returns 0 for failure apparently
        if(save_result == 0)
        {
            LOG("ERROR: SOIL failed saving screenshot {%u,%u} \"%s\""
                , surface_width, surface_height, file_path.c_str());
            LOG(" SOIL: %s", SOIL_last_result());
        }
        else
        {
            LOG("Screenshot saved to: \"%s\"", file_path.c_str());
        }
    }

    gl_viewport_t asdf_multiplat_t::screen_viewport() const
    {
        gl_viewport_t v;
        v.bottom_left = -ivec2(surface_width, surface_height) / 2;
        v.size = uvec2(surface_width, surface_height);

        return v;
    }



    ///
    /// Asdf Renderer
    ///

    ASDFM_API gl_vertex_spec_<vertex_attrib::position3_t> asdf_renderer_t::quad_vertex_t::vertex_spec;


    asdf_renderer_t::asdf_renderer_t(void* _gl_context)
    : gl_state(_gl_context)
    , render_target(app.settings.resolution_width, app.settings.resolution_height) //use settings resolution, which can differ from surface size
    {   
        render_target.texture.name = "asdf main render target texture";
    }

    /// Putting this in an init func, since it relies on app.renderer being fully constructed for the 
    /// GL_State proxy global to work (mostly related to creating a shader)
    void asdf_renderer_t::init()
    {
        ASSERT(!CheckGLError(), "Error before asdf_renderer_t::init()");

        render_target.init();
        GL_State->bind(render_target); //push this on as the base of the fbo stack. It'll sit there

        auto shader_path = find_folder("shaders");
        screen_shader = Content.create_shader_highest_supported(shader_path, "passthrough", "textured");

        {
            const quad_vertex_t quad_verts[] =
            {
                quad_vertex_t{vec3(-0.5f, -0.5f, 0.0f)},
                quad_vertex_t{vec3( 0.5f, -0.5f, 0.0f)},
                quad_vertex_t{vec3(-0.5f,  0.5f, 0.0f)},
                quad_vertex_t{vec3(-0.5f,  0.5f, 0.0f)},
                quad_vertex_t{vec3( 0.5f, -0.5f, 0.0f)},
                quad_vertex_t{vec3( 0.5f,  0.5f, 0.0f)}
            };


            quad.draw_mode = GL_TRIANGLES;
            quad.vbo.usage = GL_STATIC_DRAW;
            quad.initialize(screen_shader);
            quad.set_data(quad_verts, 6);
        }
        {
            const quad_vertex_t box_verts[] =
            {
                  quad_vertex_t{vec3{-0.5f, -0.5f, 0.5f}} //bottom left
                , quad_vertex_t{vec3{-0.5f,  0.5f, 0.5f}} //top left
                , quad_vertex_t{vec3{ 0.5f,  0.5f, 0.5f}} //top right
                , quad_vertex_t{vec3{ 0.5f, -0.5f, 0.5f}} //bottom right
            };

            box.draw_mode = GL_LINE_LOOP;
            box.vbo.usage = GL_STATIC_DRAW;
            box.initialize(screen_shader);
            box.set_data(box_verts, 4);
        }

        gl_state.unbind_vao();
        gl_state.unbind_vbo();

        ASSERT(!CheckGLError(), "Error initializing renderer");
    }

    void asdf_renderer_t::resize(uint32_t w, uint32_t h)
    {
        render_target.texture.write(nullptr, w, h);

        //rebind render target fbo so the correct gl viewport is set
        //easier than holding onto the bottom-most stack element and changing the vp
        GL_State->unbind_fbo();
        GL_State->bind(render_target);
    }

    void asdf_renderer_t::pre_render()
    {
        // Render to to frameBuffer
        
        //glBindRenderbufferEXT(GL_RENDERBUFFER, renderDepthBuffer);

        glClearColor(gl_clear_color.r
                   , gl_clear_color.g
                   , gl_clear_color.b
                   , gl_clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDisable(GL_CULL_FACE); //keep this off until I want to start optimizing things, otherwise I may wonder why things aren't rendering
    }

    void asdf_renderer_t::post_render()
    {
        ASSERT(GL_State->current_framebuffer() == render_target.fbo.id, "There shouldn't be any leftover FBOs in the stack");
        
        scoped_fbo_t scoped(0,  0,0, app.surface_width, app.surface_height);

        GL_State->bind(screen_shader);
        glUniform4f(screen_shader->uniform("Color"), 1.0f, 1.0f, 1.0f, 1.0f);


        //reset every frame in case other parts of the app use screen shader for rendering things to texture
        screen_shader->world_matrix = mat4();
        screen_shader->view_matrix = mat4();
        screen_shader->projection_matrix = glm::ortho<float>(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
        screen_shader->update_wvp_uniform();

        

        glClearColor(0.0f
                   , 0.0f
                   , 0.0f
                   , 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, render_target.texture.texture_id);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //glDisable(GL_CULL_FACE);

        quad.render();

        ASSERT(!CheckGLError(), "Error in post_render()");

        SDL_GL_SwapWindow(app.main_window);
    }
}
