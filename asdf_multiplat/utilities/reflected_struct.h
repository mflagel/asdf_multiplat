#pragma once

#include "stdafx.h"
#include <array>
#include "cJSON/cJSON.h"

namespace asdf
{
  //   struct introspective_var_t
  //   {
		// virtual void load_from(cJSON*) = 0;
  //   };

    template <class T> //todo: take type name str as arg?
    struct introspective_var_// : introspective_var_t
    {
        T& member_var;
        const char* member_name;

        introspective_var_(const char* _name, T& _var)
        : /*introspective_var_t()
        , */member_name(_name)
        , member_var(_var)
        {}

        // virtual ~introspective_var_() = default;

        // virtual
        void load_from(const cJSON * const data)
        {
            member_var = cJSON_GetObjectItem(data, member_name);
        }
    };

    struct introspective_struct_t
    {
        // virtual 
        void load_from(const char* json_str)
        {
			cJSON* data = cJSON_Parse(json_str);
            load_from(data);
            cJSON_Delete(data);
        }
    };


#define MEMBERS_MACRO(members) \
    std::array<const char*, PP_NARG(members)> = {{arg1_type}};  \
    std::array<const char*, PP_NARG(members)> = {{arg1_name}};
    //---

#define STRUCT_MACRO_THING(struct_name, arg1_type, arg1_name)   \
        struct struct_name                                      \
        {                                                       \
            constexpr std::string struct_id = #struct_name;

            std::array<const char*, 1> types = {{"asdf"}};            \
            std::array<const char*, 1> names = {{"asdfasdf"}};        \
                                                                \
            arg1_type arg1_name;                                \

            to_JSON()
            {
                cJSON *root;

                ASSERT(types.count() == names.count());
                for(size_t i = 0; i < types.count(); ++i)
                {
                    cJSON_AddItemToObject(root, types[i], names[i]);
                }

                cJSON, *fmt;
                root = cJSON_CreateObject();  
                cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
                cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());
                cJSON_AddStringToObject(fmt, "type", "rect");
                cJSON_AddNumberToObject(fmt, "width", 1920);
                cJSON_AddNumberToObject(fmt, "height", 1080);
                cJSON_AddFalseToObject (fmt, "interlace");
                cJSON_AddNumberToObject(fmt, "frame rate", 24);
            }
        };
        //----

    // STRUCT_MACRO_THING(thing_class, int, something);


    enum serializable_types_e
    {

    }

    struct struct_name
    {

		std::array<const char*, 1> member_types = {{ "int" }};
		std::array<const char*, 1> member_names = {{ "test_int" }};
                    
		// struct_name()
		// {
		// 	member_types[0] = "int";
		// 	member_names[0] = "test_int";
		// }

        int test_int;
    };
}