#include "palette_widget.h"
#include "ui_palette_widget.h"

#include "asdf_multiplat/data/texture_bank.h"

palette_widget::palette_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::palette_widget)
{
    ui->setupUi(this);
}

palette_widget::~palette_widget()
{
    delete ui;
}

void palette_widget::build_from_texture_bank(asdf::data::texture_bank_t const& texture_bank)
{
//    for(auto const& tex : texture_bank)
//    {
//        palette_entry_t entry;

//        //entries.push_back();
//    }
}
