#ifndef SPLINE_SETTINGS_WIDGET_H
#define SPLINE_SETTINGS_WIDGET_H

#include <QWidget>

#include <hexmap/data/spline.h>

namespace Ui {
class spline_settings_widget_t;
}

namespace color_widgets
{
    struct ColorWheel;
    struct ColorPreview;
}


class spline_settings_widget_t : public QWidget
{
    Q_OBJECT

public:
    explicit spline_settings_widget_t(QWidget *parent = 0);
    ~spline_settings_widget_t();

    void set_from_spline(asdf::hexmap::data::spline_t const&);
    void set_from_spline(asdf::hexmap::data::line_node_t const&, asdf::hexmap::data::spline_t::interpolation_e);

private:
    Ui::spline_settings_widget_t *ui;
    color_widgets::ColorWheel* color_wheel = nullptr;
    color_widgets::ColorPreview* color_preview = nullptr;

    friend class MainWindow;

signals:
    void colorSelected(QColor);
};

#endif // SPLINE_SETTINGS_WIDGET_H
