#include "object_properties_widget.h"
#include "ui_object_properties_widget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include <limits>

#include <hexmap/editor/main/editor.h>

template <typename QtType, typename ValType>
QtType* add_component_to_layout(QLayout* layout, object_properties_widget_t* op_wgt = nullptr)
{
    QtType* thing = new QtType();

    if(op_wgt)
    {
        QObject::connect(thing, static_cast<void(QtType::*)(ValType)>(&QtType::valueChanged),
            [op_wgt](ValType v)
            {
                Q_UNUSED(v);
                op_wgt->property_changed();
            }
        );
    }

    layout->addWidget(thing);
    return thing;
}

//static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged)

QLayout* layout_from_component_type(editable_component_type_e composite_type, object_properties_widget_t* op_wgt)
{
    auto* layout = new QHBoxLayout();

    switch(composite_type)
    {
        case component_int_8:   //[[FALLTHROUGH]] ///FIXME fallthrough in clang
        case component_int_32:  //[[FALLTHROUGH]]
        case component_int_64:  //[[FALLTHROUGH]]
        case component_uint_8:  //[[FALLTHROUGH]]
        case component_uint_32: //[[FALLTHROUGH]]
        case component_uint_64:
        {
            add_component_to_layout<QSpinBox, int>(layout, op_wgt);
            return layout;
        }

        case component_float_32:
        case component_float_64:
        {
            add_component_to_layout<QDoubleSpinBox, double>(layout, op_wgt);
            return layout;
        }

        case component_vec4: //[[FALLTHROUGH]]
        {
            add_component_to_layout<QDoubleSpinBox, double>(layout, op_wgt);
        }
        case component_vec3: //[[FALLTHROUGH]]
        {
            add_component_to_layout<QDoubleSpinBox, double>(layout, op_wgt);
        }
        case component_vec2:
        {
            add_component_to_layout<QDoubleSpinBox, double>(layout, op_wgt);
            add_component_to_layout<QDoubleSpinBox, double>(layout, op_wgt);
            break;
        }
        default: EXPLODE("Invalid component type");
    }

    return layout;
}


/// Member Func Implementations
object_properties_widget_t::object_properties_widget_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::object_properties_widget_t)
{
    ui->setupUi(this);

    for(size_t i = 0; i < components.size(); ++i)
    {
        components[i] = nullptr;
    }
}

object_properties_widget_t::~object_properties_widget_t()
{
    delete ui;
}

/*
  In theory I could use Sets to solve this, where each object type has a set of named editable components
  If the user selects multiple objects of different type, I take the intersection of the component sets and use
  that as the set of fields to allow editing of

  actually I could use the union instead of the intersection, since when I set object values from editable fields
  I'm ignoring irrelevant components anyways

  I still have to figure out how to give editable fields blank values when selecting multiple objects that have different
  initial values for the property field (ex: display a blank position when selecing two objects in different places)
*/
void object_properties_widget_t::set_from_object_selection(asdf::hexmap::editor::object_selection_t& selection, std::vector<asdf::hexmap::data::map_object_t>& objects)
{
    object_selection = &selection;
    object_list = &objects;

    //delete all component edit fields
    delete ui->object_properties_layout;
    ui->object_properties_layout = new QFormLayout();
    ui->properties_holder->addLayout(ui->object_properties_layout);

    //clear component pointers (they're pointing to dead data now anyways)
    for(size_t i = 0; i < components.size(); ++i)
    {
        components[i] = nullptr;
    }

    bool has_objs = object_selection->object_indices.size() > 0;
    bool has_nodes = false;

    if(has_objs && !has_nodes)
    {
        add_fields_map_object();
        size_t ind = *object_selection->object_indices.begin();
        ASSERT(object_list->size() >= ind, "there is no object at index %zu", ind);
        set_from_map_object(object_list->at(ind));
    }
    else if(has_nodes)
    {
        add_fields_line_node();
    }

    ASSERT(object_selection, "");
}


void object_properties_widget_t::add_component_field(obj_component_e component)
{
    auto* properties = ui->object_properties_layout;

    ASSERT(!components[component], "component already added");

    components[component] = layout_from_component_type(obj_component_types[component], this);
    properties->addRow(QString(obj_component_names[component]), components[component]);
}

void object_properties_widget_t::add_fields_map_object()
{
    using namespace glm;

    //asdf::hexmap::data::map_object_t* obj = nullptr;
    //if(object_selection->object_indices != 0)

    add_component_field(obj_position);
    add_component_field(obj_size);
    add_component_field(obj_color);
    add_component_field(obj_rotation);
}

void object_properties_widget_t::add_fields_line_node()
{

}

void object_properties_widget_t::set_from_map_object(asdf::hexmap::data::map_object_t const& obj)
{
    set_component_sub_value<QDoubleSpinBox>(obj_position, 0, obj.position.x);
    set_component_sub_value<QDoubleSpinBox>(obj_position, 1, obj.position.y);

    set_component_sub_value<QDoubleSpinBox>(obj_size, 0, obj.size_d2.x * 2);
    set_component_sub_value<QDoubleSpinBox>(obj_size, 1, obj.size_d2.y * 2);

    set_component_sub_value<QDoubleSpinBox>(obj_color, 0, obj.color.r);
    set_component_sub_value<QDoubleSpinBox>(obj_color, 1, obj.color.g);
    set_component_sub_value<QDoubleSpinBox>(obj_color, 2, obj.color.b);
//    set_component_sub_value<QDoubleSpinBox>(obj_color, 3, obj.color.a);

    set_component_value<QDoubleSpinBox>(obj_rotation, obj.rotation);
}

void object_properties_widget_t::set_map_object_values(asdf::hexmap::data::map_object_t& obj)
{
    obj.position.x = get_component_sub_value<QDoubleSpinBox, float>(obj_position, 0);
    obj.position.y = get_component_sub_value<QDoubleSpinBox, float>(obj_position, 1);

    obj.size_d2.x = get_component_sub_value<QDoubleSpinBox, float>(obj_size, 0) / 2.0f;
    obj.size_d2.y = get_component_sub_value<QDoubleSpinBox, float>(obj_size, 1) / 2.0f;

    obj.color.r = get_component_sub_value<QDoubleSpinBox, float>(obj_color, 0);
    obj.color.g = get_component_sub_value<QDoubleSpinBox, float>(obj_color, 1);
    obj.color.b = get_component_sub_value<QDoubleSpinBox, float>(obj_color, 2);
//    obj.color.a = get_component_sub_value<QDoubleSpinBox, float>(obj_color, 3);

    obj.rotation = get_component_value<QDoubleSpinBox, float>(obj_rotation);
}


void object_properties_widget_t::property_changed()
{
    ASSERT(object_selection, "");
    ASSERT(object_list, "");

    size_t ind = *object_selection->object_indices.begin();
    ASSERT(object_list->size() >= ind, "");

    set_map_object_values(object_list->at(ind));
    emit objects_modified();
}
