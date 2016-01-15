#pragma once

#include "asdf_multiplat.h"
#include "camera.h"

#include "polygon.h"
#include "polygon_path.h"
#include "polygon_state_machine.h"
#include "metaballs.h"
// #include "ui_base.h"

namespace asdf
{
namespace projector_fun
{
    struct projector_fun_t : asdf_specific_t
    {
        camera_t camera;
        std::shared_ptr<shader_t> shader = nullptr;

        ui_polygon_t test_polygon;
        polygon_state_machine_t test_machine;

        bezier_polygon_path_t test_bezier_path;
        ui_polygon_t bezier_path_polygon;

        metaballs_t metaballs;

        projector_fun_t();
        ~projector_fun_t();

        void init();

        void update(float dt);
        void render();

        void render_test_polygon();
        void render_bezier_curves();
        void render_metaballs();
    };
}
}