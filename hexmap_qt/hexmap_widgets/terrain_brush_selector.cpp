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
    ui->sb_hexagon_radius->setMinimum(1);
    ui->sb_hexagon_radius->setMaximum(max_hex_radius);
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