#include "terrain_brush_selector.h"
#include "ui_terrain_brush_selector.h"

terrain_brush_selector_t::terrain_brush_selector_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::terrain_brush_selector_t)
{
    ui->setupUi(this);

    //ui->listView->setVisible(false);
}

terrain_brush_selector_t::~terrain_brush_selector_t()
{
    delete ui;
}

void terrain_brush_selector_t::set_brush_tab(tabs_e tab)
{
    ui->brush_builder_frame->setVisible(tab == brush_settings);
    ui->brush_list->setVisible(tab == brush_palette);
}