#include "export_image_dialog.h"
#include "ui_export_image_dialog.h"

export_image_dialog_t::export_image_dialog_t(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::export_image_dialog_t)
{
    ui->setupUi(this);
}

export_image_dialog_t::~export_image_dialog_t()
{
    delete ui;
}
