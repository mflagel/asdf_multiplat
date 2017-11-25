#include "snap_points_widget.h"
#include "ui_snap_points_widget.h"

#include <hexmap/editor/main/editor.h>

using hex_snap_flags_t = asdf::hexmap::hex_snap_flags_t;

snap_points_widget_t::snap_points_widget_t(asdf::hexmap::editor::editor_t& _editor, QWidget *parent)
    : QWidget(parent)
    , editor(_editor)
    , ui(new Ui::snap_points_widget_t)
{
    ui->setupUi(this);

    connect(ui->btn_toggle_snap, &QPushButton::toggled, this, &snap_points_widget_t::toggle_snap);

    ui->btn_toggle_snap->setEnabled(true);
}

snap_points_widget_t::~snap_points_widget_t()
{
    delete ui;
}


hex_snap_flags_t snap_points_widget_t::snap_flags_from_button_states() const
{
    hex_snap_flags_t flags = 0;

    using namespace asdf::hexmap;

    flags |= hex_snap_center      * ui->btn_snap_center->isChecked();
    flags |= hex_snap_vertex      * ui->btn_snap_verts->isChecked();
    flags |= hex_snap_edge_center * ui->btn_snap_edge_midpoint->isChecked();

    return flags;
}


void snap_points_widget_t::toggle_snap(bool checked)
{
    ui->btn_snap_center->setEnabled(checked);
    ui->btn_snap_verts->setEnabled(checked);
    ui->btn_snap_edge_midpoint->setEnabled(checked);
    ui->sld_snap_threshold->setEnabled(checked);
    ui->dsb_snap_threshold->setEnabled(checked);

    editor.snap_mode = snap_flags_from_button_states();
}
