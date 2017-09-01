#pragma once

#include <vector>
#include <map>
#include <memory>

//#include "asdf_multiplat/data/gl_resources.h"

DIAGNOSTIC_PUSH
DIAGNOSTIC_IGNORE(-Wcomment)

class FTFont;

namespace asdf {

    struct shader_t;

    constexpr size_t MAX_SPRITES = 4096;


    /************************************************************************/
    /* Rectangle Structs
    /************************************************************************/
    struct rect_t {
        int x;
        int y;
        int width;
        int height;
        rect_t(int _x, int _y, int _width, int _height)
            : x(_x)
            , y(_y)
            , width(_width)
            , height(_height)
        {}
    };

    struct rectf_t {
        float x, y, width, height;
        rectf_t(float _x, float _y, float _width, float _height)
            : x(_x)
            , y(_y)
            , width(_width)
            , height(_height)
            {}
        rectf_t(glm::vec2 const& pos, glm::vec2 const& size)
            :x(pos.x), y(pos.y), width(size.x), height(size.y) {}

        rectf_t(const rectf_t& rect) = default;
    };


    /************************************************************************/
    /* Sprite
    /* Stores information used to render a sprite in a spritebatch
    /************************************************************************/
    struct sprite_t {
        std::shared_ptr<texture_t> texture;
        glm::vec2   position;
        rect_t      src_rect;
        color_t     color;
        glm::vec2   scale;
        float       rotation;

        sprite_t(std::shared_ptr<texture_t> const& _texture, glm::vec2 const& _position
               , rect_t const& _src_rect, color_t _color = glm::vec4(1.0f)
               , glm::vec2 const& _scale = glm::vec2(1, 1), float _rotation = 0)
            : texture(_texture)
            , position(_position)
            , src_rect(_src_rect)
            , color(_color)
            , scale(_scale)
            , rotation(_rotation) 
        {}
    };

    struct text_sprite_t
    {
        FTFont* font = nullptr;
        size_t face_size;
        std::string str;
        glm::vec2   position;
        color_t     color;
        float       rotation;

        text_sprite_t(std::string _str, FTFont* _font, size_t _face_size, glm::vec2 _position, color_t _color = color_t(1.0f), float _rotation = 0)
        : str(_str)
        , font(_font)
        , face_size(_face_size)
        , position(_position)
        , color(_color)
        , rotation(_rotation) 
        {}
    };


    /************************************************************************/
    /* SpriteVertex
    /* Stores the data used in each vertex of the spritebatch shader
    /************************************************************************/
    struct sprite_vertex_t {
        glm::vec2   position;
        glm::vec2   tex_coord;
        color_t     color;
    };

    /************************************************************************/
    /* SpriteBatch
    /************************************************************************/
    class spritebatch_t {
    public:
        bool debugging_sprites = false;
        std::shared_ptr<shader_t> spritebatch_shader = nullptr;

    private:        
        uint32_t vertex_buffer = 0;
        uint32_t index_buffer = 0;
        bool has_begun = false;

        //might be faster to store it all in one big vector and then sort it afterwards
        std::map<std::shared_ptr<texture_t>, std::vector<sprite_t>> sprite_map; //organizes the sprites by textureID
        std::vector<text_sprite_t> batched_text;

    public:
        spritebatch_t();
        ~spritebatch_t();

    public:
        void begin(glm::mat4 const& view_matrix = glm::mat4());
        void begin(glm::mat4 const& view_matrix, glm::mat4 const& projection_matrix);        

        //void draw(sprite_t const& sprite);
        void draw(std::shared_ptr<texture_t> const& texture, rectf_t dest_rect, color_t const& color = glm::vec4(1.0f), float rotation = 0);
        void draw(std::shared_ptr<texture_t> const& texture, glm::vec2 const& position, color_t const& color = color_t(1.0f), glm::vec2 const& scale = glm::vec2(1, 1), float rotation = 0);
        void draw(std::shared_ptr<texture_t> const& texture, glm::vec2 const& position, rect_t const& src_rect, color_t const& color = glm::vec4(1.0f), glm::vec2 const& scale = glm::vec2(1, 1), float rotation = 0);

        void draw_text(std::string const& text, FTFont* font, size_t face_size, glm::vec2 const& position, color_t color = color_t(1.0f), float rotation = 0);

        void end();

    private:
        void render_batch(std::shared_ptr<texture_t> const& texture);
            void render_debug_sprite(sprite_vertex_t const* sprite_verts);
            void generate_vertex_data(size_t num_batched_sprites);
    };

}

DIAGNOSTIC_POP
