#ifndef OBJECT_PROPERTIES_WIDGET_H
#define OBJECT_PROPERTIES_WIDGET_H

#include <QWidget>

namespace Ui {
class object_properties_widget_t;
}

namespace asdf {
namespace hexmap {
namespace data
{
    struct map_object_t;
}
namespace editor
{
    struct object_selection_t;
}}}

enum editable_component_type_e
{
      component_int_8
    , component_int_32
    , component_int_64
    , component_uint_8
    , component_uint_32
    , component_uint_64
    , component_float_32
    , component_float_64
    , component_vec2
    , component_vec3
    , component_vec4
};

class object_properties_widget_t : public QWidget
{
    Q_OBJECT

public:
    explicit object_properties_widget_t(QWidget *parent = 0);
    ~object_properties_widget_t();

    void update(asdf::hexmap::editor::object_selection_t* new_selection);

private:
    asdf::hexmap::editor::object_selection_t* object_selection = nullptr;
    //std::vector<asdf::hexmap::data::map_object_t>* object_list;

    Ui::object_properties_widget_t *ui;

    void add_fields_map_object();
    void add_fields_line_node();
};

#endif // OBJECT_PROPERTIES_WIDGET_H
