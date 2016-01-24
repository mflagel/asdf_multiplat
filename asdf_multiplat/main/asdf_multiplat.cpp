#include "stdafx.h"
#include "asdf_multiplat.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


using namespace asdf::util;
using namespace std;

namespace asdf {

    asdf_multiplat_t app;

    asdf_multiplat_t::asdf_multiplat_t()
        : main_window(0)
        , gl_context(0)
        , main_surface(0)
        , framebuffer(nullindex)
        , render_target(nullptr)
        , render_depth_buffer(nullindex)
        , quad_VBO(nullindex)
        , running(true)
        , WINDOW_TITLE("Asdf Multiplat Application Thing")
        , in_focus(true)
        , spritebatch(nullptr)
    {
        std::set_terminate(util::terminate_handler);
        signal(SIGINT,util::interrupt_handler);
    }

    // asdf_specific_t::~asdf_specific_t()
    // {}

    void asdf_multiplat_t::init(std::string _exec_dir) {
        LOG("--- Initializing This Crazy Contraption ---");

        exec_dir = _exec_dir;
        working_directory = get_current_working_directory();

        LOG("Exec Dir: %s", exec_dir.c_str());
        LOG("Working Dir: %s", working_directory.c_str());

        init_SDL();
        init_openGL();

        Content.init();
        spritebatch = make_shared<spritebatch_t>();

        main_view = make_shared<ui_view_t>(glm::vec2(0, 0), glm::vec2(settings.resolution_width, settings.resolution_height));
    }

    asdf_multiplat_t::~asdf_multiplat_t() {
        /*glDeleteBuffers(1, &quad_VBO);
        glDeleteRenderbuffers(1, &render_depth_buffer);
        glDeleteFramebuffers(1, &frame_buffer);*/

        SDL_FreeSurface(main_surface);
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(main_window);
        SDL_Quit();
    }


    void asdf_multiplat_t::update() {
        uint currentTicks = SDL_GetTicks();
        float timeElapsed = (currentTicks - prev_ticks) / 1000.0f;

        frame_times[++frame_time_index] = timeElapsed;
        frame_time_index %= frame_times.size();

        prev_ticks = currentTicks;

        if (!in_focus)
            return;

        ASSERT(specific, "app.specific not assigned");
        specific->update(timeElapsed);

        main_view->update(timeElapsed);

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
        // Render to to frameBuffer
        //glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        //glBindRenderbufferEXT(GL_RENDERBUFFER, renderDepthBuffer);
        // glClearColor(0.5f, 0.75f, 0.9f, 1.0f); //cornflower blue makin it feel like XNA
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearColor(gl_clear_color.r
                   , gl_clear_color.g
                   , gl_clear_color.b
                   , gl_clear_color.a);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, settings.resolution_width, settings.resolution_height);

        
        glDisable(GL_CULL_FACE);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);

        glUseProgram(0);
        //Content.fonts["arial"]->Render("TEST POST PLEASE IGNORE", -1, FTPoint(0, 500));

        {
            spritebatch->begin();
            ASSERT(!CheckGLError(), "gl error after begin");
            main_view->render(glm::vec3(0, 0, 0), glm::mat3(), color_t(1.0f));
            ASSERT(!CheckGLError(), "GL error after main_view->render() ");
            spritebatch->end();
        }

        glUseProgram(0);

        if (render_debug_views)
            render_debug();

        ASSERT(specific, "app.specific not assigned");
        specific->render();

        SDL_GL_SwapWindow(main_window);
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

        main_view->render_debug();

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
                    save_screenshot("screenshot_test__");
                }
                break;

            case SDL_MOUSEMOTION:
                event->motion.x -= settings.resolution_width  / 2.0f;
                event->motion.y = settings.resolution_height / 2.0f - event->motion.y;
                //LOG("x: %i,   y: %i", event->motion.x, event->motion.y);
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                event->button.x -= settings.resolution_width / 2.0f;
                event->motion.y = settings.resolution_height / 2.0f - event->motion.y;
                break;

            case SDL_MOUSEWHEEL:
                break;

            case SDL_WINDOWEVENT:
                break;
        }

        specific->on_event(event);
        main_view->on_event(event, glm::vec3(), glm::mat3());
    }

    void asdf_multiplat_t::init_SDL() {
        //init video subsystem
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            LOG("Could not initialize SDL: %s.", SDL_GetError());
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
        uint flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        if (settings.fullscreen)
            flags |= SDL_WINDOW_FULLSCREEN;
        if (settings.borderless)
            flags |= SDL_WINDOW_BORDERLESS;

        main_window = SDL_CreateWindow(WINDOW_TITLE.c_str(),
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        settings.resolution_width, settings.resolution_height,
                                        flags);
        checkSDLError(__LINE__);
        ASSERT(main_window != 0, "Unable to create window");

        //create OpenGL context
        gl_context = SDL_GL_CreateContext(main_window);
        checkSDLError(__LINE__);

        ASSERT(gl_context != nullptr, "Unable to create OpenGL Context");

        prev_ticks = SDL_GetTicks();
        //SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

        SDL_GL_SetSwapInterval(1);
        LOG("SDL Initialized");
    }

    void asdf_multiplat_t::init_openGL() {
        GLenum err = glewInit();
        ASSERT(err == GLEW_OK, "Error: %s\n", glewGetErrorString(err));
        ASSERT(!CheckGLError(), "OpenGL Error after glewInit");

        LOG("### Using OpenGL Version: %s ###\n\n", glGetString(GL_VERSION));

        /*const GLfloat quadVerts[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
        };

        glGenBuffers(1, &quad_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (!InitFrameBuffer())
            return false;
            */
        gl_initialized = true;
        LOG("OpenGL Initialized");
        LOG("--Supported OpenGL Extensions--")

        gl_extensions = tokenize((char*)(glGetString(GL_EXTENSIONS)), " ");

        for(auto const& ext : gl_extensions)
        {
            LOG("%s", ext.c_str());
        }

        LOG("--");
    }

    void asdf_multiplat_t::init_framebuffer() {

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
            file_path.c_str(),
            SOIL_SAVE_TYPE_BMP,
            0, 0, settings.resolution_width, settings.resolution_height
        );

        //soil returns 0 for failure apparently
        if(save_result == 0)
        {
            LOG("ERROR: SOIL failed saving screenshot {%zu,%zu} \"%s\""
                , settings.resolution_width, settings.resolution_height, file_path.c_str());
            LOG(" SOIL: %s", SOIL_last_result());
        }
        else
        {
            LOG("Screenshot saved to: \"%s\"", file_path.c_str());
        }
    }
}