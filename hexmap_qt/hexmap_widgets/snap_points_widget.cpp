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

    ///defaults
    ui->btn_toggle_snap->setChecked(true);
    ui->btn_snap_center->setChecked(true);
    ui->btn_snap_verts->setChecked(true);
    ui->sld_snap_threshold->setValue(50);
    ui->dsb_snap_threshold->setValue(0.5);
    ///

    connect(ui->btn_toggle_snap, &QPushButton::toggled, this, &snap_points_widget_t::toggle_snap);

    connect(ui->btn_toggle_snap,        &QPushButton::toggled, this, &snap_points_widget_t::snap_settings_changed);
    connect(ui->btn_snap_center,        &QPushButton::toggled, this, &snap_points_widget_t::snap_settings_changed);
    connect(ui->btn_snap_verts,         &QPushButton::toggled, this, &snap_points_widget_t::snap_settings_changed);
    connect(ui->btn_snap_edge_midpoint, &QPushButton::toggled, this, &snap_points_widget_t::snap_settings_changed);

    connect(ui->sld_snap_threshold, &QSlider::valueChanged,
            [this](int val) {
                ui->dsb_snap_threshold->setValue(val/100.0f);
            });
    connect(ui->dsb_snap_threshold, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &snap_points_widget_t::snap_settings_changed);

    toggle_snap(ui->btn_toggle_snap->isChecked());
}

snap_points_widget_t::~snap_points_widget_t()
{
    delete ui;
}

void snap_points_widget_t::snap_settings_changed()
{
    editor.set_snap_settings(snap_flags_from_button_states(), ui->dsb_snap_threshold->value());
}


hex_snap_flags_t snap_points_widget_t::snap_flags_from_button_states() const
{
    using namespace asdf::hexmap;
    hex_snap_flags_t flags = 0;

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

    snap_settings_changed();
}
