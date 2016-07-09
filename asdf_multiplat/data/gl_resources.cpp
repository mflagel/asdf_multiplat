#include "stdafx.h"
#include "gl_resources.h"

namespace asdf
{
    gl_state_t GL_State;


    void gl_state_t::bind(vao_t const& vao)
    {
        glBindVertexArray(vao.id);
        current_vao = vao.id;
    }

    void gl_state_t::bind(vbo_t const& vbo)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
        current_vbo = vbo.id;
    }

    void gl_state_t::unbind_vao()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_vao = 0;
    }

    void gl_state_t::unbind_vbo()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_vbo = 0;
    }

    void gl_state_t::unbind_shader()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_vbo = 0;
    }


    bool gl_state_t::assert_sync()
    {
        //todo:
        //check vbo
        //check vao

        return true;
    }
}