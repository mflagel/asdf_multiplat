#include "stdafx.h"
#include "character.h"
#include "skill_list_layout.h"

using namespace std;
using namespace glm;

#include "cJSON/cJSON.h"

#pragma clang diagnostic ignored "-Wunused-macros"

namespace asdf
{
namespace gurps_track
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



    /*******************************/
    /*****  UTIL
    /*******************************/
    cJSON* date_t::to_JSON() const
    {
        cJSON* date_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(date_json, "day",   day);
        cJSON_AddNumberToObject(date_json, "month", month);
        cJSON_AddNumberToObject(date_json, "year",  year);

        return date_json;
    }

    void date_t::from_JSON(cJSON* root)
    {
        CJSON_GET_INT(day);
        CJSON_GET_INT(month);
        CJSON_GET_INT(year);
    }

    /*******************************/
    /*****  CHARACTER
    /*******************************/
    cJSON* char_point_transaction_t::to_JSON() const
    {
        cJSON* root = cJSON_CreateObject();
        CJSON_ADD_ITEM(date);
        CJSON_ADD_NUMBER(amount);
        CJSON_ADD_STR(description);

        return root;
    }

    void char_point_transaction_t::from_JSON(cJSON* root)
    {
        CJSON_GET_ITEM(date);
        CJSON_GET_INT(amount);
        CJSON_GET_STR(description);
    }

    cJSON* status_effect_t::to_JSON() const
    {
        cJSON* root = cJSON_CreateObject();
        CJSON_ADD_STR(name);
        CJSON_ADD_NUMBER(effect_type);
        CJSON_ADD_NUMBER(type_index);
        CJSON_ADD_NUMBER(amount);

        return root;
    }

    void status_effect_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        CJSON_GET_ENUM_INT(effect_type, effect_type_e);
        CJSON_GET_INT(type_index);
        CJSON_GET_INT(amount);
    }

    cJSON* trait_t::to_JSON() const
    {
        cJSON* root = cJSON_CreateObject();
        CJSON_ADD_STR(name);
        CJSON_ADD_STR(description);
        CJSON_ADD_NUMBER(point_cost);
        CJSON_ADD_ITEM_ARRAY(status_effects);

        return root;
    }

    void trait_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        CJSON_GET_STR(description);
        CJSON_GET_INT(point_cost);
        CJSON_GET_ITEM_VECTOR(status_effects);
    }

    cJSON* race_t::to_JSON() const
    {
        cJSON* root = cJSON_CreateObject();
        CJSON_ADD_STR(name);
        CJSON_ADD_STR(description);
        CJSON_ADD_ITEM_ARRAY(traits);

        return root;
    }

    void race_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        CJSON_GET_STR(description);
        CJSON_GET_ITEM_VECTOR(traits);
    }

    cJSON* character_t::to_JSON() const
    {
        cJSON* root = cJSON_CreateObject();

        //stick this meta-information first
        CJSON_ADD_NUMBER(save_format_version);
        CJSON_ADD_NUMBER(version);
        CJSON_ADD_ITEM(date_modified);

        CJSON_ADD_ITEM_ARRAY(point_transactions);

        CJSON_ADD_STR(name);
        CJSON_ADD_STR(description);
        CJSON_ADD_STR(portrait_filepath);
        CJSON_ADD_INT_ARRAY(base_stats);
        CJSON_ADD_NUMBER(current_fatigue);
        CJSON_ADD_NUMBER(current_HP);

        CJSON_ADD_ITEM(race);

        CJSON_ADD_ITEM_ARRAY(traits);

        return root;
    }

    void character_t::from_JSON(cJSON* root)
    {
        ASSERT(root, "Cannot load character data from null json object");

        auto fmt_json = cJSON_GetObjectItem(root, "save_format_version");
        ASSERT(fmt_json, "Failed to load the save format json data");
        size_t fmt_version = fmt_json->valueint;

        ASSERT(save_format_version == fmt_version, "version mismatch, todo: handle such things");

        CJSON_GET_INT(version);
        CJSON_GET_ITEM(date_modified);

        CJSON_GET_ITEM_VECTOR(point_transactions);

        CJSON_GET_STR(name);
        CJSON_GET_STR(description);
        CJSON_GET_STR(portrait_filepath);
        CJSON_GET_INT_ARRAY(base_stats);
        CJSON_GET_INT(current_fatigue);
        CJSON_GET_INT(current_HP);

        CJSON_GET_ITEM(race);

        CJSON_GET_ITEM_VECTOR(traits);
    }

    /*******************************/
    /*****  SKILL
    /*******************************/

    cJSON* skill_default_t::to_JSON() const
    {
        CJSON_CREATE_ROOT();
        CJSON_ADD_STR(name);
        CJSON_ADD_INT(penalty);
        return root;
    }
    void skill_default_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        CJSON_GET_INT(penalty);
    }

    cJSON* skill_t::to_JSON() const
    {
        CJSON_CREATE_ROOT();
        CJSON_ADD_STR(name);
        CJSON_ADD_STR(description);
        CJSON_ADD_INT(difficulty);
        CJSON_ADD_STR(defaults)
        return root;
    }
    void skill_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        CJSON_GET_STR(description);
        CJSON_GET_ENUM_INT(difficulty, skill_difficulty_e);
        CJSON_GET_STR(defaults);
    }



    void skill_library_t::from_JSON(cJSON* root)
    {
        CJSON_GET_ITEM_VECTOR(skills);
    }
}
}