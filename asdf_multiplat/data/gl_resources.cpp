#include "stdafx.h"
#include "gl_resources.h"

namespace asdf
{
    gl_state_t GL_State;


    void gl_state_t::bind(vao_t const& vao)
    {
        LOG_IF(current_vao == vao.id, "vao %i already in use", vao);
        glBindVertexArray(vao.id);
        current_vao = vao.id;
    }

    void gl_state_t::bind(vbo_t const& vbo)
    {
        LOG_IF(current_vbo == vbo.id, "vbo %i already in use", vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
        current_vbo = vbo.id;
    }

    void gl_state_t::bind(std::shared_ptr<shader_t> const& shader)
    {
        //LOG_IF(current_shader == shader->shader_program_id, "shader \'%s\' already in use", shader->name.c_str());
        glUseProgram(shader->shader_program_id);
        current_shader = shader->shader_program_id;
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
        //check shader

        return true;
    }
}