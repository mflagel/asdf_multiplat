#ifndef SNAP_POINTS_WIDGET_H
#define SNAP_POINTS_WIDGET_H

#include <QWidget>
#include <hexmap/data/hex_util.h>

namespace Ui {
    class snap_points_widget_t;
}

namespace asdf{ namespace hexmap{ namespace editor {
    struct editor_t;
}}}

class snap_points_widget_t : public QWidget
{
    Q_OBJECT

public:
    explicit snap_points_widget_t(asdf::hexmap::editor::editor_t&, QWidget *parent = 0);
    ~snap_points_widget_t();

    asdf::hexmap::editor::editor_t& editor;

private:
    Ui::snap_points_widget_t *ui;

    asdf::hexmap::hex_snap_flags_t snap_flags_from_button_states() const;

private slots:
    void snap_settings_changed();
    void toggle_snap(bool checked);
};

#endif // SNAP_POINTS_WIDGET_H
