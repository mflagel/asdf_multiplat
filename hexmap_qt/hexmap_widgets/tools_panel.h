#ifndef TOOLS_PANEL_H
#define TOOLS_PANEL_H

#include <QWidget>

namespace Ui {
class tools_panel;
}


class tools_panel_t : public QWidget
{
    Q_OBJECT

public:
    explicit tools_panel_t(QWidget* parent = 0);
    ~tools_panel_t();

private:
    Ui::tools_panel *ui;
};

#endif // TOOLS_PANEL_H
