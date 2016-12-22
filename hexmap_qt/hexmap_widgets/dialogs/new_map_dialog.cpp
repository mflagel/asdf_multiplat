#include "new_map_dialog.h"
#include "ui_new_map_dialog.h"

new_map_dialog_t::new_map_dialog_t(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::new_map_dialog_t)
{
    ui->setupUi(this);
}

new_map_dialog_t::~new_map_dialog_t()
{
    delete ui;
}
