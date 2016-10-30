#ifndef LAYERS_WIDGET_H
#define LAYERS_WIDGET_H

#include <QWidget>

namespace Ui {
class layers_widget;
}

class layers_widget : public QWidget
{
    Q_OBJECT

public:
    explicit layers_widget(QWidget *parent = 0);
    ~layers_widget();

private:
    Ui::layers_widget *ui;
};

#endif // LAYERS_WIDGET_H
