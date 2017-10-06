#include "snap_points_widget.h"
#include "ui_snap_points_widget.h"

snap_points_widget_t::snap_points_widget_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::snap_points_widget_t)
{
    ui->setupUi(this);
}

snap_points_widget_t::~snap_points_widget_t()
{
    delete ui;
}
