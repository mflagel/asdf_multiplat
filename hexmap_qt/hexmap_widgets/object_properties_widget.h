#ifndef OBJECT_PROPERTIES_WIDGET_H
#define OBJECT_PROPERTIES_WIDGET_H

#include <array>

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

enum obj_component_e
{
      obj_position
    , obj_size
    , obj_color
    , obj_rotation
    , num_obj_components
};

constexpr std::array<const char*, num_obj_components> obj_component_names =
{
      "Position"
    , "Size"
    , "Color"
    , "Rotation"
};

constexpr std::array<editable_component_type_e, num_obj_components> obj_component_types =
{
      component_vec2
    , component_vec2
    , component_vec4
    , component_float_32
};

class object_properties_widget_t : public QWidget
{
    Q_OBJECT

public:
    explicit object_properties_widget_t(QWidget *parent = 0);
    ~object_properties_widget_t();

    void set_from_object_selection(asdf::hexmap::editor::object_selection_t&, std::vector<asdf::hexmap::data::map_object_t>&);

private:
    asdf::hexmap::editor::object_selection_t* object_selection = nullptr;
    std::vector<asdf::hexmap::data::map_object_t>* object_list;

    std::array<QLayout*, num_obj_components> components;

    Ui::object_properties_widget_t *ui;

    void add_component_field(obj_component_e);

    void add_fields_map_object();
    void add_fields_line_node();

    template <typename QtType, typename T>
    void set_component_value(obj_component_e component, T value)
    {
        set_component_sub_value<QtType>(component, 0, value);
    }

    template <typename QtType, typename T>
    void set_component_sub_value(obj_component_e component, size_t index, T value)
    {
        ///FIXME modify asdf_multiplat so that I can use ASSERT here without failing to compile
        if(!components[component])
            return;
        if(components[component]->count() <= index)
            return;
        //ASSERT(components[component], "");
        //ASSERT(components[component]->count() > index, "");
        qobject_cast<QtType*>(components[component]->itemAt(index)->widget())->setValue(value);
    }

    void set_from_map_object(asdf::hexmap::data::map_object_t const& obj);
};

#endif // OBJECT_PROPERTIES_WIDGET_H
