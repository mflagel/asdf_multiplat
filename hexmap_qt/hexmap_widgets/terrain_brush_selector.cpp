#include "terrain_brush_selector.h"
#include "ui_terrain_brush_selector.h"

#include <asdf_multiplat/main/asdf_defs.h>

namespace
{
    double circle_radius_slider_divisor = 10.0; //divide the slider amount by this to get the current circle radius
    int max_hex_radius_slider = 10;
    int max_hex_radius = 999;
}

terrain_brush_selector_t::terrain_brush_selector_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::terrain_brush_selector_t)
{
    ui->setupUi(this);


    //hide/disable buttons for unfinished features
    ui->brush_builder_tabs->removeTab(3); //bitmap
    ui->brush_builder_tabs->removeTab(2); //circular

    /// Hexagonal Brush
    ui->sld_hexagon_radius->setMinimum(1);
    ui->sld_hexagon_radius->setMaximum(max_hex_radius_slider);
    ui->sld_hexagon_radius->setPageStep(max_hex_radius_slider/4);

    ui->sb_hexagon_radius->setMinimum(1);
    ui->sb_hexagon_radius->setMaximum(max_hex_radius);
    ui->sb_hexagon_radius->setSingleStep(1);

    connect(ui->sld_hexagon_radius, &QSlider::valueChanged, ui->sb_hexagon_radius, &QSpinBox::setValue);
    connect(ui->sb_hexagon_radius, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int v)
    {
        set_brush(hexagonal);
    });

    /// Rectangular Brush
    connect(ui->sb_brush_width, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int v)
    {
        set_brush(rectangular); //will update rectangular brush
    });

    connect(ui->sb_brush_height, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int v)
    {
        set_brush(rectangular); //will update rectangular brush
    });

    /// Circular Brush
    /*connect(ui->sld_circle_radius, &QSlider::valueChanged, [this](int v){
            ui->dsb_circle_radius->setValue(static_cast<double>(v) / circle_radius_slider_divisor);
        });

    connect(ui->dsb_circle_radius, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [this](double value)
        {
            set_brush(circular); //will update circular brush
        });
        */
}

terrain_brush_selector_t::~terrain_brush_selector_t()
{
    delete ui;
}


int terrain_brush_selector_t::get_brush_size() const
{
    switch(cur_brush_type)
    {
        case hexagonal:
            return ui->sld_hexagon_radius->value();
        case rectangular:
        {
            auto w = ui->sb_brush_width->value();
            auto h = ui->sb_brush_height->value();
            return glm::max(w,h);
            break;
        }
        case circular:
            return convert_integer<float,int>(ui->sld_circle_radius->value());
            break;
    }
}

void terrain_brush_selector_t::set_brush_size(int _size)
{
    switch(cur_brush_type)
    {
        case hexagonal:
            ui->sld_hexagon_radius->setValue(_size);
            break;
        case rectangular:
            ui->sb_brush_width->setValue(_size);
            ui->sb_brush_height->setValue(_size);
            break;
        case circular:
            ui->sld_circle_radius->setValue(convert_integer<int,float>(_size));
            break;
    }
}

void terrain_brush_selector_t::shrink_brush(int32_t amt)
{
    adjust_brush_size(-amt);
}

void terrain_brush_selector_t::grow_brush(int32_t amt)
{
    adjust_brush_size(amt);
}

void terrain_brush_selector_t::adjust_brush_size(int32_t amt)
{
    switch(cur_brush_type)
    {
        case hexagonal:
        {
            auto v = ui->sld_hexagon_radius->value();
            ui->sld_hexagon_radius->setValue(v + amt);
            break;
        }
        case rectangular:
        {
            {
                auto v = ui->sb_brush_width->value();
                ui->sb_brush_width->setValue(v + amt);
            }
            {
                auto v = ui->sb_brush_height->value();
                ui->sb_brush_height->setValue(v + amt);
            }
        }
            break;
        case circular:
        {
            auto v = ui->sld_circle_radius->value();
            ui->sld_circle_radius->setValue(v + amt);
            break;
        }
    }
}

void terrain_brush_selector_t::shrink_brush()
{
    adjust_brush_size(false);
}

void terrain_brush_selector_t::grow_brush()
{
    adjust_brush_size(true);
}

/// no-arg func just uses the pageStep or singleStep
void terrain_brush_selector_t::adjust_brush_size(bool grow)
{
    int mult = grow ? 1 : -1;

    switch(cur_brush_type)
    {
        case hexagonal:
        {
            auto v = ui->sld_hexagon_radius->value();
            auto s = ui->sld_hexagon_radius->pageStep();
            ui->sld_hexagon_radius->setValue(v + s * mult);
            break;
        }
        case rectangular:
        {
            {
                auto v = ui->sb_brush_width->value();
                auto s = ui->sb_brush_width->singleStep();
                ui->sb_brush_width->setValue(v + s * mult);
            }
            {
                auto v = ui->sb_brush_height->value();
                auto s = ui->sb_brush_height->singleStep();
                ui->sb_brush_height->setValue(v + s * mult);
            }
        }
            break;
        case circular:
        {
            auto v = ui->sld_circle_radius->value();
            auto s = ui->sld_circle_radius->pageStep();
            ui->sld_circle_radius->setValue(v + s * mult);
            break;
        }
    }
}


void terrain_brush_selector_t::brush_type_changed(int tab_index)
{
    auto b = static_cast<brush_types_e>(tab_index);

    if(b != cur_brush_type)
        set_brush(b);
}

void terrain_brush_selector_t::set_brush(brush_types_e brush_type)
{
    cur_brush_type = brush_type;

    using namespace asdf::hexmap::data;

    switch(brush_type)
    {
    case hexagonal:
        terrain_brush = terrain_brush_hexagon(hexagon_brush_radius());
        break;
    case rectangular:
        terrain_brush = terrain_brush_t(rect_brush_size());
        break;
    case circular:
        terrain_brush = terrain_brush_circle(circular_brush_radius());
        break;
    case bitmap:
        //TODO
        break;
    default:
        EXPLODE("invalid brush type");
    }

    emit custom_brush_changed(terrain_brush);
}


int terrain_brush_selector_t::hexagon_brush_radius() const
{
    return ui->sb_hexagon_radius->value() - 1;
}

glm::uvec2 terrain_brush_selector_t::rect_brush_size() const
{
    return glm::uvec2(ui->sb_brush_width->value(), ui->sb_brush_height->value());
}

float terrain_brush_selector_t::circular_brush_radius() const
{
    return ui->dsb_circle_radius->value();
}