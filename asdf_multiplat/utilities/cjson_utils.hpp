#pragma once
#include "main/asdf_defs.h"

#include <array>

#include "utilities/utilities.h"


DIAGNOSTIC_PUSH
DIAGNOSTIC_IGNORE(-Wsign-conversion)
DIAGNOSTIC_IGNORE(-Wshorten-64-to-32)
DIAGNOSTIC_IGNORE(-Wsign-conversion)
DIAGNOSTIC_IGNORE(-Wdouble-promotion)
#include "cJSON/cJSON.h"
DIAGNOSTIC_POP

DIAGNOSTIC_PUSH
DIAGNOSTIC_IGNORE(-Wunused-macros);


constexpr std::array<char const*, cJSON_Object+2> cJSON_type_strings =
{
    "cJSON_False"
  , "cJSON_True"
  , "cJSON_NULL"
  , "cJSON_Number"
  , "cJSON_String"
  , "cJSON_Array"
  , "cJSON_Object"
  , "Invalid cJSON Type"
};

#define CJSON_OBJ(node_name, obj_name) cJSON_GetObjectItem(node_name, #obj_name)
#define CJSON_INT(node_name, int_name) CJSON_OBJ(node_name, int_name)->valueint
#define CJSON_FLOAT(node_name, float_name) static_cast<float>(CJSON_OBJ(node_name, float_name)->valuedouble)
#define CJSON_DOUBLE(node_name, double_name) CJSON_OBJ(node_name, double_name)->valuedouble
#define CJSON_STR(node_name, str_name) CJSON_OBJ(node_name, str_name)->valuestring
#define CJSON_ENUM(node_name, int_name, enum_type) static_cast<enum_type>(CJSON_INT(node_name, int_name))

#define CJSON_VALUE_ARRAY(container_node, container, valuetype, resize_container)     \
    {                                                                     \
        size_t len = cJSON_GetArraySize(container_node);                  \
                                                                          \
        if(resize_container)                                              \
        {                                                                 \
            container.clear();                                            \
            container.resize(len);                                        \
        }                                                                 \
        else                                                              \
        {                                                                 \
            ASSERT(container.size() == len, "");                          \
        }                                                                 \
                                                                          \
        for(size_t i = 0; i < len; ++i)                                   \
        {                                                                 \
            cJSON* json = cJSON_GetArrayItem(container_node, i);          \
            container[i] = json->valuetype;                               \
        }                                                                 \
    }
//---



#define CJSON_GET_INT(int_name)    int_name = CJSON_INT(root, int_name)
#define CJSON_GET_FLOAT(float_name) float_name = CJSON_FLOAT(root, float_name)
#define CJSON_GET_DOUBLE(double_name) int_name = CJSON_DOUBLE(root, double_name)
#define CJSON_GET_STR(str_name)    str_name = CJSON_STR(root, str_name)
#define CJSON_GET_ITEM(obj_name) obj_name.from_JSON(cJSON_GetObjectItem(root, #obj_name));

#define CJSON_GET_ENUM_INT(int_name, enum_type) int_name = CJSON_ENUM(int_name, enum_type);

#define CJSON_GET_VALUE_ARRAY(container, valuetype, resize_container)     \
    {                                                                     \
        cJSON* container_json = cJSON_GetObjectItem(root, #container);  \
        CJSON_VALUE_ARRAY(container_json, container, valuetype, resize_container) \
    }
//---

#define CJSON_GET_INT_ARRAY(container)    CJSON_GET_VALUE_ARRAY(container, valueint, false);
#define CJSON_GET_DOUBLE_ARRAY(container) CJSON_GET_VALUE_ARRAY(container, valuedouble, false);
#define CJSON_GET_STR_ARRAY(container)    CJSON_GET_VALUE_ARRAY(container, valuestring, false);

#define CJSON_GET_INT_VECTOR(container)    CJSON_GET_VALUE_ARRAY(container, valueint, true);
#define CJSON_GET_DOUBLE_VECTOR(container) CJSON_GET_VALUE_ARRAY(container, valuedouble, true);
#define CJSON_GET_STR_VECTOR(container)    CJSON_GET_VALUE_ARRAY(container, valuestring, true);

#define CJSON_GET_ITEM_ARRAY(container) \
    {                                                                        \
        cJSON* container##_json = cJSON_GetObjectItem(root, #container);     \
        size_t len = cJSON_GetArraySize(container##_json);                   \
        for(size_t i = 0; i < len; ++i)                                      \
        {                                                                    \
            container[i].from_JSON(cJSON_GetArrayItem(container##_json, i)); \
        }                                                                    \
    }
//---


#define CJSON_FOR_EACH_ITEM_VECTOR(container, obj_from_json_func) \
    { \
        cJSON* container##_json = cJSON_GetObjectItem(root, #container);     \
        size_t len = cJSON_GetArraySize(container##_json);                   \
                                                                             \
        for(size_t i = 0; i < len; ++i)                                      \
        {                                                                    \
            obj_from_json_func(cJSON_GetArrayItem(container##_json, i));     \
        }                                                                    \
    }
//---


#define CJSON_GET_ITEM_VECTOR(container) \
    { \
        cJSON* container##_json = cJSON_GetObjectItem(root, #container);     \
        size_t len = cJSON_GetArraySize(container##_json);                   \
                                                                             \
        container.clear();                                                   \
        container.resize(len);                                               \
                                                                             \
        for(size_t i = 0; i < len; ++i)                                      \
        {                                                                    \
            container[i].from_JSON(cJSON_GetArrayItem(container##_json, i)); \
        }                                                                    \
    }
//---

/*
*/
#define CJSON_CREATE_ROOT() cJSON* root = cJSON_CreateObject()
#define CJSON_ADD_BLANK_ITEM(root, child) \
    cJSON* child##_json = cJSON_CreateObject(); \
    cJSON_AddItemToObject(root, #child, child##_json);
//---
#define CJSON_ADD_ITEM(obj)  cJSON_AddItemToObject(root, #obj, obj.to_JSON());


#define CJSON_ADD_NUMBER(num_name)  cJSON_AddNumberToObject(root, #num_name, num_name);
#define CJSON_ADD_INT(num_name)     CJSON_ADD_NUMBER(num_name)
#define CJSON_ADD_STR(str_name)     cJSON_AddStringToObject(root, #str_name, (str_name##.c_str() == nullptr) ? "" : str_name##.c_str());
#define CJSON_ADD_CSTR(str_name)    cJSON_AddStringToObject(root, #str_name, str_name);

#define CJSON_ITEM_ARRAY(container_name)                              \
    cJSON* container_name##_json = cJSON_CreateArray();               \
    for(auto const& thing : container_name)                           \
    { cJSON_AddItemToArray(container_name##_json, thing.to_JSON()); }
//---

#define CJSON_ADD_INT_ARRAY(container)   cJSON_AddItemToObject(root, #container, cJSON_CreateIntArray(container.data(), container.size()));

#define CJSON_ADD_ITEM_ARRAY(container_name)      \
    CJSON_ITEM_ARRAY(container_name);                       \
    cJSON_AddItemToObject(root, #container_name, container_name##_json);
DIAGNOSTIC_POP


namespace asdf
{
    inline std::string json_to_string(cJSON* j)
    {
        char* cjson_cstr = cJSON_Print(j);
        std::string str(cjson_cstr);
        free(cjson_cstr);

        return str;
    }

    inline std::string json_to_string_unformatted(cJSON* j)
    {
        char* cjson_cstr = cJSON_PrintUnformatted(j);
        std::string str(cjson_cstr);
        free(cjson_cstr);

        return str;
    }

    inline void json_to_file(cJSON* j, std::string const& filepath)
    {
        util::write_text_file(filepath, json_to_string(j));
    }

    inline cJSON* json_from_file(std::string const& filepath)
    {
        std::string json_str = util::read_text_file(filepath);
        return cJSON_Parse(json_str.c_str());
    }

    /// Based on cJSON_CreateStringArray in cJSON.c
    inline cJSON* cJSON_CreateStringArray(std::vector<std::string> const& strings)
    {
        int i;
        cJSON *n=0,*p=0,*a=cJSON_CreateArray();
        for(i=0;a && i<strings.size();i++)
        {
            n=cJSON_CreateString(strings[i].c_str());
            if(!i)
                a->child=n;
            else
            {
                p->next=n;
                n->prev=n;
            }
            p=n;
        }
        return a;
    }

    /// Based on cJSON_CreateStringArray in cJSON.c
    inline cJSON* cJSON_CreatePathArray(std::vector<std::experimental::filesystem::path> const& paths)
    {
        int i;
        cJSON *n=0,*p=0,*a=cJSON_CreateArray();
        for(i=0;a && i<paths.size();i++)
        {
            n=cJSON_CreateString(paths[i].string().c_str());
            if(!i)
                a->child=n;
            else
            {
                p->next=n;
                n->prev=n;
            }
            p=n;
        }
        return a;
    }
}
