#include "pallet_widget.h"
#include "ui_pallet_widget.h"

pallet_widget::pallet_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pallet_widget)
{
    ui->setupUi(this);
}

pallet_widget::~pallet_widget()
{
    delete ui;
}
