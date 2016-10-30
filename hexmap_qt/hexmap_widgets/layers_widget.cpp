#include "layers_widget.h"
#include "ui_layers_widget.h"

layers_widget::layers_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::layers_widget)
{
    ui->setupUi(this);
}

layers_widget::~layers_widget()
{
    delete ui;
}
