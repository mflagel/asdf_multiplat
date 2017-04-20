#include "stdafx.h"
#include "asdf_multiplat.h"
#include "spritebatch.h"
#include "content_manager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std;
using namespace glm;
using namespace asdf::util;

namespace asdf {

    spritebatch_t::spritebatch_t()
    {
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &index_buffer);
    }

    spritebatch_t::~spritebatch_t() {
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteBuffers(1, &index_buffer);
    }


    /************************************************************************/
    /* Spritebatch Begin
    /* Initializes a batch with an optional view and/or proj matrix
    /************************************************************************/
    void spritebatch_t::begin(glm::mat4 const& view_matrix/*mat4()*/) {
        float halfwidth = app.surface_width / 2.0f;
        float halfheight = app.surface_height / 2.0f;

        mat4 proj = ortho<float>(-halfwidth, halfwidth,
                            -halfheight, halfheight,
                            -1000, 1000);

        begin(view_matrix, proj);
    }
    void spritebatch_t::begin(glm::mat4 const& view_matrix, glm::mat4 const& projection_matrix) {
        ASSERT(spritebatch_shader, "Trying to begin() without a shader");
        ASSERT(!has_begun, "beginning spritebatch before current batch has ended");
        ASSERT(!CheckGLError(), "GL Error before spritebatch_t::begin()");

        has_begun = true;
        spritebatch_shader->view_matrix = view_matrix;
        spritebatch_shader->projection_matrix = projection_matrix;
    }


    /************************************************************************/
    /* Spritebatch Draw
    /* Add a sprite to a map indexed by texture used
    /* src_rect is relative to the bottom_left of the texture (since that's the openGL {0,0} point)
    /************************************************************************/    
    void spritebatch_t::draw(std::shared_ptr<texture_t> const& texture, rectf_t dest_rect, color_t const& color/*vec4(1.0f)*/, float rotation/*0*/) {
        ASSERT(texture != nullptr, "Drawing a sprite with a null texture");
        glm::vec2 scale(dest_rect.width / texture->get_width(), dest_rect.height / texture->get_width());
        draw(texture, glm::vec2(dest_rect.x, dest_rect.y), color, scale, rotation);
    }
    void spritebatch_t::draw(std::shared_ptr<texture_t> const& texture, glm::vec2 const& position, color_t const& color/*vec4(1.0f)*/, glm::vec2 const& scale/*vec2(1,1)*/, float rotation/*0*/) {
        ASSERT(texture != nullptr, "Drawing a sprite with a null texture");
        rect_t rect(0, 0, texture->get_width(), texture->get_height());
        draw(texture, position, rect, color, scale, rotation);
    }
    void spritebatch_t::draw(std::shared_ptr<texture_t> const& texture, glm::vec2 const& position, rect_t const& src_rect, color_t const& color/*vec4(1.0f)*/, glm::vec2 const& scale/*vec2(1,1)*/, float rotation/*0*/) {
        ASSERT(texture != nullptr, "Drawing a sprite with a null texture");
        // LOG_IF(scale.x == 0 && scale.y == 0, "Drawing a sprite with no scale");

        sprite_map[texture].push_back(sprite_t(texture, position, src_rect, color, scale, rotation));
    }    

    /************************************************************************/
    /* Spritebatch DrawText
    /* All text is batched and rendered at the end
    /*
    /* Todo: implement Z sorting so the text isn't just rendered on top
    /************************************************************************/
    void spritebatch_t::draw_text(std::string const& str, FTFont* font, size_t face_size, glm::vec2 const& position, color_t color, float rotation) {
        ASSERT(font != nullptr, "Spritebatching text without a font");

        if(str.size() > 0)
        {
            batched_text.push_back( text_sprite_t(str, font, face_size, position, color, rotation) );
        }
    }

    /************************************************************************/
    /* Spritebatch End
    /* Sets up consistent openGL state and renders out a batch for each texture
    /************************************************************************/
    void spritebatch_t::end() {
        ASSERT(has_begun, "ending spritebatch that hasn't begun");
        ASSERT(!CheckGLError(), "GL Error before spritebatch_t::end()");

        glEnable(GL_BLEND); //enable alpha blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blend function

        spritebatch_shader->use_program();

        spritebatch_shader->world_matrix = mat4(1.0f);
        spritebatch_shader->update_wvp_uniform();

        //glUniform1i(spritebatch_shader->uniforms.at("TextureMap"), 0);  //set the GLSL sampler to texture unit 0
        glActiveTexture(GL_TEXTURE0); //set current texture unit to 0
        glBindSampler(0, Content.samplers["LinearClamp"]);
        //glBindSampler(0, Content.samplers["LinearRepeat"]);

        auto textures_used = GetMapKeys<std::shared_ptr<texture_t>, std::vector <sprite_t>> (sprite_map);

        if (textures_used.size() > 0) {
            for (auto const& texture : textures_used)
                render_batch(texture);
        } else {
            // LOG("--Ended a spritebatch with no draw calls")
        }
        sprite_map.clear();


        glUseProgram(0);
        //render text
        for (text_sprite_t const& text : batched_text)
        {
            //assumes orthographic projection where 2/[0,0] = screen width and 2/[1,1] = screen_height
            float screen_halfw = 1 / spritebatch_shader->projection_matrix[0][0];
            float screen_halfh = 1 / spritebatch_shader->projection_matrix[1][1];
            glm::vec2 ftfont_pos = text.position;
            ftfont_pos.x += screen_halfw;
            ftfont_pos.y += screen_halfh;
            
            //don't render things we can't see
            if(ftfont_pos.x < (screen_halfw * 2) && ftfont_pos.y > 0)
            {
                text.font->FaceSize(text.face_size);
                //text.font->Render(text.str.c_str(), -1, FTPoint(ftfont_pos.x, ftfont_pos.y));  /// FIXME
            }
        }

        batched_text.clear();

        has_begun = false;

        ASSERT(!CheckGLError(), "GL Error in spritebatch_t::end()");
    }


    /************************************************************************/
    /* Spritebatch End
    /* Builds the sprite quads, sends them to openGL, and renders
    /* 
    /* Todo: Refactor this function and trim it down
    /************************************************************************/
    void spritebatch_t::render_batch(shared_ptr<texture_t> const& texture) 
    {
        uint16_t numBatchedSprites = 0;

        sprite_vertex_t spriteVertices[9001];
        uint16_t spriteIndices[9001];

        //todo: refactor this
        for (sprite_t const& sprite : sprite_map[texture]) {
            vec2 up(0.0f, 1.0f);
            vec2 right(1.0f, 0.0f);

            up = rotate(up, -sprite.rotation);
            right = rotate(right, -sprite.rotation);
            
            rectf_t src_rect_f(
                  static_cast<float>(sprite.src_rect.x)
                , static_cast<float>(sprite.src_rect.y)
                , static_cast<float>(sprite.src_rect.width)
                , static_cast<float>(sprite.src_rect.height)
                );

            uint16_t vertNum = numBatchedSprites * 4;
            float hwidth = src_rect_f.width  / 2.0f;
            float hheight = src_rect_f.height / 2.0f;
            float spritehwidth = hwidth * sprite.scale[0];
            float spritehheight = hheight * sprite.scale[1];

            ///FIXME precision
            float minX = src_rect_f.x / static_cast<float>(texture->get_width());
            float minY = src_rect_f.y / static_cast<float>(texture->get_height());
            float maxX = static_cast<float>(sprite.src_rect.x + sprite.src_rect.width) / static_cast<float>(texture->get_width());
            float maxY = static_cast<float>(sprite.src_rect.y + sprite.src_rect.height) / static_cast<float>(texture->get_height());


            /*  0 _________ 1
            *    |        /|
            *    |      /  |
            *    |    /    |
            *    |  /      |
            *    |/________|
            *   2           3
            */

            //v0 - top left
            spriteVertices[vertNum].position = sprite.position + right*(-spritehwidth) + up*(spritehheight);
            spriteVertices[vertNum].tex_coord = vec2(minX, maxY);
            spriteVertices[vertNum].color = sprite.color;

            //v1 - top right
            spriteVertices[vertNum + 1].position = sprite.position + right*(spritehwidth)+up*(spritehheight);
            spriteVertices[vertNum + 1].tex_coord = vec2(maxX, maxY);
            spriteVertices[vertNum + 1].color = sprite.color;

            //v2 - bottom left
            spriteVertices[vertNum + 2].position = sprite.position + right*(-spritehwidth) + up*(-spritehheight);
            spriteVertices[vertNum + 2].tex_coord = vec2(minX, minY);
            spriteVertices[vertNum + 2].color = sprite.color;

            //v3- bottom right
            spriteVertices[vertNum + 3].position = sprite.position + right*(spritehwidth)+up*(-spritehheight);
            spriteVertices[vertNum + 3].tex_coord = vec2(maxX, minY);
            spriteVertices[vertNum + 3].color = sprite.color;

            size_t indexNum = numBatchedSprites * 6;

            spriteIndices[indexNum + 0] = vertNum + 0;
            spriteIndices[indexNum + 1] = vertNum + 1;
            spriteIndices[indexNum + 2] = vertNum + 2;

            spriteIndices[indexNum + 3] = vertNum + 2;
            spriteIndices[indexNum + 4] = vertNum + 1;
            spriteIndices[indexNum + 5] = vertNum + 3;

            //--- DEBUG ---
            //todo: refactor
            if (debugging_sprites) {
                LOG_IF(CheckGLError(), "Error before drawing spritebatch debug information");

                auto passthrough = Content.shaders["passthrough"];
                passthrough->use_program();
                passthrough->world_matrix       = spritebatch_shader->world_matrix;
                passthrough->view_matrix        = spritebatch_shader->view_matrix;
                passthrough->projection_matrix  = spritebatch_shader->projection_matrix;

                passthrough->update_wvp_uniform();
                glUniform4f(passthrough->uniforms["Color"], 1.0f, 0.0f, 0.0f, 1.0f);

                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(6.0f);

                glBegin(GL_POLYGON);
                glTexCoord2f(0.01f, 0.99f); glVertex2f(spriteVertices[vertNum + 0].position.x, spriteVertices[vertNum + 2].position.y); //BOTTOM LEFT
                glTexCoord2f(0.01f, 0.01f); glVertex2f(spriteVertices[vertNum + 1].position.x, spriteVertices[vertNum + 1].position.y);  //TOP LEFT
                glTexCoord2f(0.99f, 0.01f); glVertex2f(spriteVertices[vertNum + 2].position.x, spriteVertices[vertNum + 0].position.y);   //TOP RIGHT
                glTexCoord2f(0.99f, 0.99f); glVertex2f(spriteVertices[vertNum + 3].position.x, spriteVertices[vertNum + 3].position.y);  //BOTTOM RIGHT
                glEnd();

                spritebatch_shader->use_program();
                LOG_IF(CheckGLError(), "Error drawing spritebatch debug information");
            }
            //---


            numBatchedSprites++;
        }        

        //bind/push the index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBatchedSprites * 6 * sizeof(short),
                     reinterpret_cast<void*>(spriteIndices), GL_STREAM_DRAW);

        //bind the vbo and push the vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, numBatchedSprites * 4 * sizeof(sprite_vertex_t),
                     reinterpret_cast<void*>(spriteVertices), GL_STREAM_DRAW);

        //enable position, texCoord, color
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        //set up vertex attrib size / types. stride is just the total size of the vertex
        GLsizei stride = sizeof(sprite_vertex_t);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(glm::vec2)) ); //MUST STILL PROVIDE OFFSET
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<void*>(sizeof(glm::vec2) * 2) );

        LOG_IF(CheckGLError(), "Error after setting up spritebatch vertex attributes");

        //set openGL state
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture->get_textureID()); //bind the texture. Sampler is set in End()
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);

        glDrawElements(GL_TRIANGLES, numBatchedSprites * 6, GL_UNSIGNED_SHORT, 0);

        if (debugging_sprites) {
            Content.shaders["passthrough"]->use_program();
            glUniform4f(Content.shaders["passthrough"]->uniforms["Color"], 0.0f, 0.0f, 1.0f, 1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(2.0f);

            glDrawElements(GL_TRIANGLES, numBatchedSprites * 6, GL_UNSIGNED_SHORT, 0);
        }

        //reset opengl state
        //glEnable(GL_CULL_FACE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        LOG_IF(CheckGLError(), "Error after drawing spritebatch polygons");
        ASSERT(!CheckGLError(), "");
    }

}