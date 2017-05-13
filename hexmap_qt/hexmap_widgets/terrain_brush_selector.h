#ifndef TERRAIN_BRUSH_SELECTOR_H
#define TERRAIN_BRUSH_SELECTOR_H

#include <QWidget>

#include <hexmap/data/terrain_brush.h>

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

    enum brush_types_e
    {
          circular = 0
        , rectangular
        , bitmap
    };

public:
    explicit terrain_brush_selector_t(QWidget *parent = 0);
    ~terrain_brush_selector_t();

public slots:
    void brush_type_changed(int tab_index);

private:
    void set_brush_tab(tabs_e);
    void set_brush(brush_types_e);

    float circular_brush_radius() const;
    glm::uvec2 rect_brush_size() const;

    asdf::hexmap::data::terrain_brush_t terrain_brush;
    brush_types_e cur_brush_type = circular;
    Ui::terrain_brush_selector_t *ui;
};

#endif // TERRAIN_BRUSH_SELECTOR_H
