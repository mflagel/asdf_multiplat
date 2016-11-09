#include "tools_panel.h"
#include "ui_tools_panel.h"

tools_panel_t::tools_panel_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tools_panel)
{
    ui->setupUi(this);
}

tools_panel_t::~tools_panel_t()
{
    delete ui;
}
