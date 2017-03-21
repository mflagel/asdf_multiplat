#include "object_properties_widget.h"
#include "ui_object_properties_widget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include <limits>

#include <hexmap/editor/main/editor.h>

template <editable_component_type_e component_type, typename T>
QWidget* widget_from_component_type(T initial_value = T())
{
    switch(component_type)
    {
        case component_int_8:   [[FALLTHROUGH]]
        case component_int_32:  [[FALLTHROUGH]]
        case component_int_64:  [[FALLTHROUGH]]
        case component_uint_8:  [[FALLTHROUGH]]
        case component_uint_32: [[FALLTHROUGH]]
        case component_uint_64:
        {
            auto* sb = new QSpinBox();
            sb->setMinimum(std::numeric_limits<T>::min());
            sb->setMaximum(std::numeric_limits<T>::max());
            sb->setValue(initial_value);
            return sb;
        }

        case component_float_32:
        case component_float_64:
        {
            auto* sb = new QDoubleSpinBox();
            sb->setMinimum(std::numeric_limits<T>::min());
            sb->setMaximum(std::numeric_limits<T>::max());
            sb->setValue(initial_value);
            return sb;
        }
         default: EXPLODE("TODO: implement this component type");
    }


    /*
    if(component_type >= component_vec2 && component_type <= component_vec4)
    {
        auto* layout = new QHBoxLayout();

        switch(component_type)
        {
            case component_vec4: [[FALLTHROUGH]]
            {
                auto* sb = new QDoubleSpinBox();
                sb->setValue(initial_value.w);
                layout->addItem(sb);
            }
            case component_vec3: [[FALLTHROUGH]]
            {
                auto* sb = new QDoubleSpinBox();
                sb->setValue(initial_value.z);
                layout->addItem(sb);
            }
            case component_vec2:
            {
                auto* sbx = new QDoubleSpinBox();
                auto* sby = new QDoubleSpinBox();
                sbx->setValue(initial_value.x);
                sbx->setValue(initial_value.y);
                layout->addItem(sbx);
                layout->addItem(sby);

                break;
            }
            default: EXPLODE("");
        }
        return layout;
        */
}


/// Member Func Implementations
object_properties_widget_t::object_properties_widget_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::object_properties_widget_t)
{
    ui->setupUi(this);
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
void object_properties_widget_t::update(asdf::hexmap::editor::object_selection_t* new_selection)
{
    if(new_selection)
    {
        object_selection = new_selection;
    }

    delete ui->object_properties_layout;
    ui->object_properties_layout = new QFormLayout();
    ui->properties_holder->addLayout(ui->object_properties_layout);

    bool has_objs = object_selection->object_indices.size() > 0;
    bool has_nodes = false;

    if(has_objs && !has_nodes)
    {
        add_fields_map_object();
    }
    else if(has_nodes)
    {
        add_fields_line_node();
    }

    ASSERT(object_selection, "");


}

void object_properties_widget_t::add_fields_map_object()
{
    using namespace glm;

    //asdf::hexmap::data::map_object_t* obj = nullptr;
    //if(object_selection->object_indices != 0)

    auto* layout = ui->object_properties_layout;

    //layout->addRow( "Position", widget_from_component_type<component_vec2, vec2>() );
    //layout->addRow( "Size",     widget_from_component_type<component_vec2, vec2>() );
    //layout->addRow( "Color",    widget_from_component_type<component_vec4, vec4>() );
    layout->addRow( "Rotation", widget_from_component_type<component_float_32, float>() );
}

void object_properties_widget_t::add_fields_line_node()
{

}
