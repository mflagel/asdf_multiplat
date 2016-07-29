#pragma once;

#include <vector>
#include <memory>
#include <functional>

#include <SDL2/SDL_events.h>
#include <glm/glm.hpp>

#define UI_RENDER_PARAMS glm::vec3 const& _position, glm::mat3 const& _matrix, color_t const& _color

#define BUTTON_DISABLED_COLOR  glm::vec4(0.2f)
#define BUTTON_PRESSED_COLOR   glm::vec4(0.5f)
#define BUTTON_HIGHLIGHT_COLOR glm::vec4(1.5f)

//TODO: put this and the one in stdafx somewhere better
#define nullindex 0xFFFFFFFF

/** Todo **
    Remove the render_debug() function from everything and
    just add something in ui_base_t::render to show the
    upper/lower bounds

    Might be worth to pass down transform through align()
*/

namespace asdf
{
    struct texture_t;
    struct shader_t;

    struct ui_base_t;
    struct ui_label_t;
    struct ui_button_t;

    typedef std::function<void(ui_base_t* sender)> ui_function_t;
    typedef glm::vec4       color_t;

    template <typename T>
    struct aabb_
    {
        T upper_bound;
        T lower_bound;
    };

    // TODO: rename this something less stupid;
    // I should just remove this, it's kind of lazy to begin with
    #define DEFAULT_TRANSFORM_COPYPASTA        \
        vec3 position_ = _position + position; \
        mat3 matrix_ = _matrix * matrix;       \
        color_t color_ = _color * color;

    using aabb_t = aabb_<glm::vec3>;
    using aabb2_t = aabb_<glm::vec2>;

    enum ui_alignment_e : uint32_t {
        ui_align_none = 0, // No alignment means position is world-relative not parent-relative
        ui_align_center = 1,

        ui_align_top = 2,
        ui_align_left = 4,
        ui_align_right = 8,
        ui_align_bottom = 16,

        ui_align_top_left  = ui_align_top | ui_align_left,
        ui_align_top_right = ui_align_top | ui_align_right,
        ui_align_bottom_left  = ui_align_bottom | ui_align_left,
        ui_align_bottom_right = ui_align_bottom | ui_align_right,

        ui_align_vertical_stretch = ui_align_top | ui_align_bottom,
        ui_align_horizontal_stretch = ui_align_left | ui_align_right,

        ui_align_top_stretch = ui_align_top | ui_align_horizontal_stretch,
        ui_align_left_stretch = ui_align_left | ui_align_vertical_stretch,
        ui_align_right_stretch = ui_align_right | ui_align_vertical_stretch,
        ui_align_bottom_stretch = ui_align_bottom | ui_align_horizontal_stretch,

        ui_align_fill = ui_align_center | ui_align_vertical_stretch | ui_align_horizontal_stretch,
    };

    enum ui_alignment_edge_e
    {
          ui_align_edge_center = 0
        , ui_align_edge_inside
        , ui_align_edge_outside
    };

    /// UI Base
    struct ui_base_t {
        //I could probably make one mat4 store the transform
        //and have references to different components
        //rather than storing them seperately and combining

        glm::vec3 position; //relative to parent
        glm::mat3 matrix; //rotation and scale
        color_t color;

        //ub and lb are cached values containing the world coords of the base
        glm::vec3 upper_bound;
        glm::vec3 lower_bound;

        glm::vec3 size3; //size allows getting a local AABB
        glm::vec3 halfsize3;

        ui_alignment_e alignment = ui_align_center;
        ui_alignment_edge_e alignment_edge = ui_align_edge_inside;
        glm::vec3 offset; //might remove this from base and only add to classes that need it
        glm::vec3 expansion_delta = glm::vec3(0.0f);

        ui_base_t* parent;
        std::vector<std::shared_ptr<ui_base_t>> children;

        bool visible = true;
        bool enabled = true;

        ui_base_t(glm::vec3 const& position, glm::mat3 const& matrix = glm::mat3());
        virtual ~ui_base_t();

        ui_base_t* add_child(std::shared_ptr<ui_base_t> child, size_t index = nullindex);

        template <class T, typename... Args>
        T* add_child(Args&& ...args)
        {
            return static_cast<T*>( add_child(std::make_shared<T>(std::forward<Args>(args)...)) );
        }

        ui_base_t* top_parent();

        virtual void align();
        virtual void set_size(glm::vec3 size);
        virtual void resize_to_fit();

        virtual void update(float dt);
        virtual void render(UI_RENDER_PARAMS);
        virtual void render_debug();
        virtual bool on_event(SDL_Event*, glm::vec3 const& _position, glm::mat3 const& _matrix);
        //virtual void on_mouse_event(SDL_MouseMotionEvent*);
        //virtual void on_keyboard_event(SDL_KeyboardEvent*);

        //virtual void on_mouse_event(SDL_MouseButtonEvent*);

        bool contains_point(glm::vec3 point);
        glm::vec3 get_world_position();
        glm::mat4 get_world_transform();
        aabb_t calc_aabb();
    };

    aabb_t get_bounds(std::vector<aabb_t> const& aabbs);

    /// UI View
    struct ui_view_t : ui_base_t {
        glm::vec2 size;     //could make this a ref of size3.xy
        glm::vec2 halfsize; //could make this a ref of halfsize3.xy

        ui_view_t(glm::vec2 const& position = glm::vec2(), ui_alignment_e alignment = ui_align_center);
        ui_view_t(glm::vec2 const& position, glm::vec2 const& size, ui_alignment_e alignment = ui_align_center);

        virtual void align() override;
        void set_size(glm::vec2 size);
        void set_size(float w, float h) {set_size(glm::vec2{w,h});}
        virtual void set_size(glm::vec3 size) override;

        virtual void update(float dt) override;
        virtual void render(UI_RENDER_PARAMS) override;
    };


    /// UI Image
    struct ui_image_t : ui_view_t {
        std::shared_ptr<texture_t> texture;

        //ui_image_t(std::shared_ptr<texture_t> texture, glm::vec2(position), glm::vec2(size) = glm::vec2(nullindex), ui_alignment_e alignment = ui_align_center);
        ui_image_t(std::shared_ptr<texture_t> _texture, glm::vec2 _position, glm::vec2 _size = glm::vec2(nullindex), ui_alignment_e _alignment = ui_align_center);
        //ui_image_t(color_t color, glm::vec2(position), glm::vec2(size) = glm::vec2(nullindex), ui_alignment_e alignment = ui_align_center);
        ui_image_t(color_t color, glm::vec2 _position, glm::vec2 _size = glm::vec2(nullindex), ui_alignment_e alignment = ui_align_center);
        ui_image_t(color_t color, ui_alignment_e align = ui_align_fill);

        virtual void update(float dt) override;
        virtual void render(UI_RENDER_PARAMS) override;
    };

    struct ui_slider_t : ui_base_t {
        std::shared_ptr<ui_button_t> slider_button;
        std::shared_ptr<ui_image_t>  slider_image_front;
        std::shared_ptr<ui_image_t>  slider_image_back;

        glm::vec3 slide_start;
        glm::vec3 slide_end;
        float percentage;

        ui_slider_t(std::shared_ptr<ui_button_t> button = nullptr, std::shared_ptr<ui_image_t> front_img = nullptr, std::shared_ptr<ui_image_t> back_img = nullptr);

        virtual void update(float dt) override;
        virtual void render(UI_RENDER_PARAMS) override;
        virtual bool on_event(SDL_Event*, glm::vec3 const& _position, glm::mat3 const& _matrix) override;

        ui_function_t on_start_callback;
        ui_function_t on_end_callback;
        ui_function_t on_end_inside_callback;
        ui_function_t on_changed_callback;
    };

    struct ui_scrollbar_t : ui_slider_t {
        std::shared_ptr<ui_button_t> scroll_backwards_button;
        std::shared_ptr<ui_button_t> scroll_forwards_button;

        ui_scrollbar_t(std::shared_ptr<ui_button_t> button = nullptr, std::shared_ptr<ui_image_t> front_img = nullptr, std::shared_ptr<ui_image_t> back_img = nullptr);
    };

    // I will probably deprecate this with a scroll view at some point
    struct ui_list_view_t : ui_view_t
    {
        std::vector<std::shared_ptr<ui_view_t>> cells;
        //glm::vec3 direction = glm::vec3(0.0f);
        bool vertical = true; //todo: deprecate in favour of direction

        float padding = 0.0f;

        ui_list_view_t(glm::vec2 position = glm::vec2(), glm::vec2 size = glm::vec2(), ui_alignment_e alignment = ui_align_center);
        ui_view_t* add_cell(std::shared_ptr<ui_view_t> cell);

        template <class T, typename... Args>
        T* add_cell(Args&& ...args)
        {
            return static_cast<T*>( add_cell(std::make_shared<T>(std::forward<Args>(args)...)) );
        }

        void normalize_sizes();

        virtual void align() override;
        void align_cells();

        virtual void update(float dt) override;
        virtual void render(UI_RENDER_PARAMS) override;
        virtual bool on_event(SDL_Event*, glm::vec3 const& _position, glm::mat3 const& _matrix) override;

        protected:
        float stride(std::shared_ptr<ui_view_t> const& cell) const;
    };

    struct ui_table_t : ui_list_view_t
    {
        std::vector<ui_list_view_t*> rows;
        std::vector<float> row_heights; //might do a map<int, float> to reduce size
        std::vector<float> col_widths;
        //bool use_percentage_x, use_percentage_y;

        float column_padding = 0.0f;

        ui_table_t(glm::vec2 position = glm::vec2(), glm::vec2 size = glm::vec2(), ui_alignment_e alignment = ui_align_center);
        ui_list_view_t* add_row(float height = 0.0f);

        void set_column_padding(float);

        virtual void align() override;
        // virtual void update(float dt) override;
        // virtual void render(UI_RENDER_PARAMS) override;
        // virtual void render_debug() override;
        // virtual void on_event(SDL_Event*) override;
    };
}