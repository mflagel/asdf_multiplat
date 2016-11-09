#ifndef PALLET_WIDGET_H
#define PALLET_WIDGET_H

#include <QWidget>

namespace Ui {
class pallet_widget;
}

class pallet_widget : public QWidget
{
    Q_OBJECT

public:
    explicit pallet_widget(QWidget *parent = 0);
    ~pallet_widget();

private:
    Ui::pallet_widget *ui;
};

#endif // PALLET_WIDGET_H
