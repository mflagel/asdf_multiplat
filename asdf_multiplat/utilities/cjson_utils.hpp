#pragma once
#include "stdafx.h"
#include "cJSON/cJSON.h"

CLANG_DIAGNOSTIC_PUSH;
CLANG_DIAGNOSTIC_IGNORE("-Wunused-macros");


// I dont think namespace matters for macros but whatever
namespace asdf
{
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
    //---
}


CLANG_DIAGNOSTIC_POP