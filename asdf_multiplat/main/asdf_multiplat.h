#pragma once

#include <array>
#include <vector>
#include <memory>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "data/gl_state.h"
#include "data/settings.h"

namespace asdf {

    struct texture_t;
    class spritebatch_t;
    struct ui_view_t;

    struct asdf_specific_t
    {
        virtual ~asdf_specific_t() = default;

        virtual void init() = 0;
        virtual void update(float dt) = 0;
        virtual void render() = 0;
        virtual void on_event(SDL_Event*) = 0;
    };

    struct asdf_multiplat_t 
    {
        gl_state_t gl_state;

        // std::shared_ptr<asdf_specific_t> specific;
        asdf_specific_t* specific{nullptr};
        SDL_Window*     main_window{nullptr};
        SDL_GLContext   gl_context;
        SDL_Surface*    main_surface{nullptr};
        unsigned int    prev_ticks{0};

        std::array<float, 10> frame_times;
        size_t frame_time_index = 0;

        GLuint      framebuffer{9001};
        std::shared_ptr<texture_t> render_target;
        GLuint      render_depth_buffer{9001};
        GLuint      quad_VBO{9001};
        //color_t     gl_clear_color = color_t{0.5f, 0.75f, 0.9f, 1.0f}; //cornflower blue makin it feel like XNA
        glm::vec4 gl_clear_color = glm::vec4{0.5f, 0.75f, 0.9f, 1.0f};
        
        bool running        = false;
        bool in_focus       = false;
        std::string WINDOW_TITLE = "";

        std::string exec_dir = "";
        std::string working_directory = "";

        bool render_debug_views = false;

        settings_t settings;
        // mouse_state_t mouse_state;
        std::shared_ptr<spritebatch_t> spritebatch{nullptr};
        std::shared_ptr<ui_view_t>  main_view{nullptr};

    
        asdf_multiplat_t();
        ~asdf_multiplat_t();

        void init(std::string _exec_dir);

        void init_SDL();

        void update();
        void render();
        void render_debug();
        
        void on_event(SDL_Event*);

        void save_screenshot(std::string file_path) const;
        float average_frame_time() const;
    };

    extern asdf_multiplat_t app;
}