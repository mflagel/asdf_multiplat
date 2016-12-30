#include "new_map_dialog.h"
#include "ui_new_map_dialog.h"

//todo: move to some "constants.h"?
namespace
{
    constexpr int max_map_width  = 1000;
    constexpr int max_map_height = 1000;
}

new_map_dialog_t::new_map_dialog_t(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::new_map_dialog_t)
{
    ui->setupUi(this);

    ui->spb_width-> setMaximum(max_map_width);
    ui->spb_height->setMaximum(max_map_height);
}

new_map_dialog_t::~new_map_dialog_t()
{
    delete ui;
}
