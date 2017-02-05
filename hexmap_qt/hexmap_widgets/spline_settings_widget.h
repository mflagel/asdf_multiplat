#ifndef SPLINE_SETTINGS_WIDGET_H
#define SPLINE_SETTINGS_WIDGET_H

#include <QWidget>

namespace Ui {
class spline_settings_widget_t;
}

class spline_settings_widget_t : public QWidget
{
    Q_OBJECT

public:
    explicit spline_settings_widget_t(QWidget *parent = 0);
    ~spline_settings_widget_t();

private:
    Ui::spline_settings_widget_t *ui;
};

#endif // SPLINE_SETTINGS_WIDGET_H
