#pragma once

#include <array>
#include <vector>
#include <memory>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "main/mouse.h"
#include "data/gl_state.h"
#include "data/texture.h"
#include "data/settings.h"
#include "ui/polygon.h"

namespace asdf {

    struct texture_t;
    class spritebatch_t;
    //struct ui_view_t;

    struct asdf_specific_t
    {
        virtual ~asdf_specific_t() = default;

        virtual void init() = 0;
        virtual void update(float dt) = 0;
        virtual void render() = 0;
        virtual void on_event(SDL_Event*) = 0;
    };

    struct asdf_renderer_t
    {
        gl_state_t gl_state;

        render_target_t render_target;
        //render_buffer_t render_depth_buffer;

        std::shared_ptr<shader_t> screen_shader;

        struct quad_vertex_t
        {
            static gl_vertex_spec_<vertex_attrib::position3_t> vertex_spec;
            glm::vec3 position;
        };
        rendered_polygon_<quad_vertex_t> quad;
        rendered_polygon_<quad_vertex_t> box; //4 verts, instead of two triangles

        //color_t     gl_clear_color = color_t{0.5f, 0.75f, 0.9f, 1.0f}; //cornflower blue makin it feel like XNA
        glm::vec4 gl_clear_color = glm::vec4{0.5f, 0.75f, 0.9f, 1.0f};

        asdf_renderer_t();

        void init();

        void pre_render();
        void post_render();

        glm::uvec2 render_target_size() const { return glm::uvec2(render_target.texture.width, render_target.texture.height); }
    };

    struct asdf_multiplat_t 
    {
        // std::shared_ptr<asdf_specific_t> specific;
        asdf_specific_t* specific{nullptr};
        SDL_Window*     main_window{nullptr};
        SDL_GLContext   gl_context;
        SDL_Surface*    main_surface{nullptr};
        uint32_t surface_width  = 0;
        uint32_t surface_height = 0;

        std::unique_ptr<asdf_renderer_t> renderer;

        unsigned int    prev_ticks{0};
        std::array<float, 10> frame_times;
        size_t frame_time_index = 0;
        
        bool running        = false;
        bool in_focus       = false;
        std::string WINDOW_TITLE = "";

        std::string exec_dir = "";
        std::string working_directory = "";

        bool render_debug_views = false;

        settings_t settings;
        std::shared_ptr<spritebatch_t> spritebatch{nullptr};

        mouse_input_t mouse_state;

    
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

        gl_viewport_t screen_viewport() const;
        glm::uvec2 render_target_size() const { return renderer->render_target_size(); }
    };

    extern asdf_multiplat_t app;
}