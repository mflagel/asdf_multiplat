#include "object_properties_widget.h"
#include "ui_object_properties_widget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include <limits>

#include <hexmap/editor/main/editor.h>


QLayout* layout_from_component_type(editable_component_type_e composite_type)
{
    auto* layout = new QHBoxLayout();

    switch(composite_type)
    {
        case component_int_8:   [[FALLTHROUGH]]
        case component_int_32:  [[FALLTHROUGH]]
        case component_int_64:  [[FALLTHROUGH]]
        case component_uint_8:  [[FALLTHROUGH]]
        case component_uint_32: [[FALLTHROUGH]]
        case component_uint_64:
        {
            layout->addWidget(new QSpinBox());
            return layout;
        }

        case component_float_32:
        case component_float_64:
        {
            layout->addWidget(new QDoubleSpinBox());
            return layout;
        }

        case component_vec4: [[FALLTHROUGH]]
        {
            auto* sb = new QDoubleSpinBox();
            layout->addWidget(sb);
        }
        case component_vec3: [[FALLTHROUGH]]
        {
            auto* sb = new QDoubleSpinBox();
            layout->addWidget(sb);
        }
        case component_vec2:
        {
            auto* sbx = new QDoubleSpinBox();
            auto* sby = new QDoubleSpinBox();
            //todo: set defaults
            layout->addWidget(sbx);
            layout->addWidget(sby);

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

    components[component] = layout_from_component_type(obj_component_types[component]);
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

void object_properties_widget_t::set_from_map_object(asdf::hexmap::data::map_object_t const& obj)
{
    set_component_sub_value<QDoubleSpinBox>(obj_position, 0, obj.position.x);
    set_component_sub_value<QDoubleSpinBox>(obj_position, 1, obj.position.y);

    set_component_sub_value<QDoubleSpinBox>(obj_size, 0, obj.size_d2.x * 2);
    set_component_sub_value<QDoubleSpinBox>(obj_size, 1, obj.size_d2.y * 2);

    set_component_sub_value<QDoubleSpinBox>(obj_color, 0, obj.color.r);
    set_component_sub_value<QDoubleSpinBox>(obj_color, 1, obj.color.g);
    set_component_sub_value<QDoubleSpinBox>(obj_color, 2, obj.color.b);
    set_component_sub_value<QDoubleSpinBox>(obj_color, 3, obj.color.a);

    set_component_value<QDoubleSpinBox>(obj_rotation, obj.rotation);
}

void object_properties_widget_t::add_fields_line_node()
{

}
