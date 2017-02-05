#include "spline_settings_widget.h"
#include "ui_spline_settings_widget.h"

#include "hexmap/data/spline.h"

//using spline_t = hexmap::data::spline_t;
using namespace asdf::hexmap::data;

namespace
{
    constexpr float min_line_thickness =  0.1f;  //is also the spinner increment amount
    constexpr float max_line_thickness = 10.0f;
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

    //todo: color
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
