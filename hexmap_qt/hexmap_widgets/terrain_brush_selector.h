#ifndef TERRAIN_BRUSH_SELECTOR_H
#define TERRAIN_BRUSH_SELECTOR_H

#include <QWidget>

namespace Ui {
class terrain_brush_selector_t;
}

class terrain_brush_selector_t : public QWidget
{
    Q_OBJECT

    enum tabs_e
    {
          brush_settings = 0
        , brush_palette
    };

public:
    explicit terrain_brush_selector_t(QWidget *parent = 0);
    ~terrain_brush_selector_t();

private:
    void set_brush_tab(tabs_e);

    Ui::terrain_brush_selector_t *ui;
};

#endif // TERRAIN_BRUSH_SELECTOR_H
