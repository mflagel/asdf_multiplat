#include "spline_settings_widget.h"
#include "ui_spline_settings_widget.h"

#include "hexmap/data/spline.h"

#include <ColorWheel>
#include <ColorPreview>

//using spline_t = hexmap::data::spline_t;
using namespace asdf::hexmap::data;


namespace
{
    constexpr float min_line_thickness =  0.1f;  //is also the spinner increment amount
    constexpr float max_line_thickness = 10.0f;

    const QColor default_spline_color(0, 128, 255);
}

spline_settings_widget_t::spline_settings_widget_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::spline_settings_widget_t)
{
    ui->setupUi(this);

    for(size_t i = 0; i < spline_t::num_interp_types; ++i)
    {
        ui->InterpolationDropDown->insertItem(i, QString(spline_interpolation_names[i]));
    }

    ui->LineThicknessSpinner->setRange(min_line_thickness, max_line_thickness);
    ui->LineThicknessSpinner->setSingleStep(min_line_thickness);

    //disabled until implemented in splines
    ui->MiterJoin->setEnabled(false);
    ui->RoundJoin->setEnabled(false);
    ui->BevelJoin->setEnabled(false);

    color_wheel   = new color_widgets::ColorWheel();
    color_wheel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    color_wheel->setColor(default_spline_color);

    color_preview = new color_widgets::ColorPreview();
    color_preview->setColor(default_spline_color);

    connect(color_wheel, &color_widgets::ColorWheel::colorChanged, [this](QColor c){color_preview->setColor(c);});

    connect(color_wheel, &color_widgets::ColorWheel::colorSelected,
            [this](QColor c) {
                emit colorSelected(c);
            });

    ui->colors_layout->addWidget(color_wheel);
    ui->colors_layout->addWidget(color_preview);
}

void spline_settings_widget_t::set_from_spline(spline_t const& spline)
{
    ASSERT(spline.nodes.size() > 0, "Cant get style from a spline with no nodes");
    set_from_spline(spline.nodes[0], spline.spline_type);
}

void spline_settings_widget_t::set_from_spline(line_node_t const& node_style, spline_t::interpolation_e interpolation)
{
    ui->InterpolationDropDown->setCurrentIndex((int)interpolation);
    ui->LineThicknessSpinner->setValue(node_style.thickness);

    //todo: color
}

spline_settings_widget_t::~spline_settings_widget_t()
{
    delete ui;
}
