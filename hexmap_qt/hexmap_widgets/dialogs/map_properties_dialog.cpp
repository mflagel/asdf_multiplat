#include "map_properties_dialog.h"
#include "ui_map_properties_dialog.h"

map_properties_dialog_t::map_properties_dialog_t(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::map_properties_dialog_t)
{
    ui->setupUi(this);
}

map_properties_dialog_t::~map_properties_dialog_t()
{
    delete ui;
}
