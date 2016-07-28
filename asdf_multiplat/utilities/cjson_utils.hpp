#pragma once

#include "cJSON/cJSON.h"

CLANG_DIAGNOSTIC_PUSH
CLANG_DIAGNOSTIC_IGNORE("-Wunused-macros")


// I dont think namespace matters for macros but whatever
namespace asdf
{
    #define CJSON_GET_INT(int_name)    int_name = cJSON_GetObjectItem(root, #int_name)->valueint;
    #define CJSON_GET_FLOAT(float_name) float_name = static_cast<float>(cJSON_GetObjectItem(root, #float_name)->valuedouble);
    #define CJSON_GET_DOUBLE(int_name) int_name = cJSON_GetObjectItem(root, #int_name)->valuedouble;
    #define CJSON_GET_STR(str_name)    str_name = cJSON_GetObjectItem(root, #str_name)->valuestring;
    #define CJSON_GET_ITEM(obj_name) obj_name.from_JSON(cJSON_GetObjectItem(root, #obj_name));

    #define CJSON_GET_ENUM_INT(int_name, enum_type) int_name = static_cast<enum_type>(cJSON_GetObjectItem(root, #int_name)->valueint);

    #define CJSON_GET_VALUE_ARRAY(container, valuetype)                       \
        {                                                                     \
            cJSON* container##_json = cJSON_GetObjectItem(root, #container);  \
            size_t len = cJSON_GetArraySize(container##_json);                \
            ASSERT(container.size() == len, "");                              \
            for(size_t i = 0; i < len; ++i)                                   \
            {                                                                 \
                cJSON* json = cJSON_GetArrayItem(container##_json, i);        \
                container[i] = json->valuetype;                               \
            }                                                                 \
        }
    //---

    #define CJSON_GET_INT_ARRAY(container)    CJSON_GET_VALUE_ARRAY(container, valueint);
    #define CJSON_GET_DOUBLE_ARRAY(container) CJSON_GET_VALUE_ARRAY(container, valuedouble);
    #define CJSON_GET_STR_ARRAY(container)    CJSON_GET_VALUE_ARRAY(container, valuestring);

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