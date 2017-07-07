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

    enum brush_types_e
    {
          hexagonal = 0
        , rectangular
        , circular
        , bitmap
    };

public:
    explicit terrain_brush_selector_t(QWidget *parent = 0);
    ~terrain_brush_selector_t();

public slots:
    void brush_type_changed(int tab_index);

signals:
    void custom_brush_changed(asdf::hexmap::data::terrain_brush_t const& brush);

private:
    void set_brush(brush_types_e);

    int hexagon_brush_radius() const;
    float circular_brush_radius() const;
    glm::uvec2 rect_brush_size() const;

    asdf::hexmap::data::terrain_brush_t terrain_brush;
    brush_types_e cur_brush_type = circular;
    Ui::terrain_brush_selector_t *ui;
};

#endif // TERRAIN_BRUSH_SELECTOR_H
