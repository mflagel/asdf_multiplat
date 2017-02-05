#include "spline_settings_widget.h"
#include "ui_spline_settings_widget.h"

#include "hexmap/data/spline.h"

using spline_t = hexmap::data::spline_t;

spline_settings_widget_t::spline_settings_widget_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::spline_settings_widget_t)
{
    ui->setupUi(this);

    //for(size_t i = spline_t::)
}

spline_settings_widget_t::~spline_settings_widget_t()
{
    delete ui;
}
