#ifndef SNAP_POINTS_WIDGET_H
#define SNAP_POINTS_WIDGET_H

#include <QWidget>

namespace Ui {
class snap_points_widget_t;
}

class snap_points_widget_t : public QWidget
{
    Q_OBJECT

public:
    explicit snap_points_widget_t(QWidget *parent = 0);
    ~snap_points_widget_t();

private:
    Ui::snap_points_widget_t *ui;
};

#endif // SNAP_POINTS_WIDGET_H
