#include "stdafx.h"
#include "character.h"
#include "ui/skill_list.h"

using namespace std;
using namespace glm;

#include "asdf_multiplat/utilities/cjson_utils.hpp"

namespace gurps_track
{
namespace data
{

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

        CJSON_ADD_ITEM_ARRAY(skills);
        CJSON_ADD_ITEM_ARRAY(spells);

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

        CJSON_GET_ITEM_VECTOR(skills);
        // CJSON_GET_ITEM_VECTOR(spells);
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
        CJSON_ADD_INT(base_stat);
        CJSON_ADD_INT(difficulty);
        CJSON_ADD_STR(defaults)
        return root;
    }
    void skill_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        CJSON_GET_STR(description);
        CJSON_GET_ENUM_INT(base_stat, base_stat_e);
        CJSON_GET_ENUM_INT(difficulty, skill_difficulty_e);
        CJSON_GET_STR(defaults);
    }

    void skill_list_t::from_JSON(cJSON* root)
    {
        auto& skills = *this;
        CJSON_GET_ITEM_VECTOR(skills);
    }

    cJSON* learned_skill_t::to_JSON() const
    {
        CJSON_CREATE_ROOT();
        CJSON_ADD_ITEM(skill);
        CJSON_ADD_INT(num_improvements);
        return root;
    }

    void learned_skill_t::from_JSON(cJSON* root)
    {
        CJSON_GET_ITEM(skill);
        CJSON_GET_INT(num_improvements);
    }

    /*******************************/
    /*****  SPELL
    /*******************************/

    cJSON* spell_t::to_JSON() const
    {
        CJSON_CREATE_ROOT();
        CJSON_ADD_STR(name);
        CJSON_ADD_STR(description);
        CJSON_ADD_INT(difficulty);
        CJSON_ADD_STR(initial_cost);
        CJSON_ADD_STR(maintenance_cost);
        CJSON_ADD_STR(duration);
        CJSON_ADD_STR(time_to_cast);
        CJSON_ADD_INT(duration_type);
        CJSON_ADD_INT(defense_type);
        CJSON_ADD_STR(info);
        return root;
    }
    void spell_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        CJSON_GET_STR(description);
        CJSON_GET_ENUM_INT(difficulty, skill_difficulty_e);
        CJSON_GET_STR(initial_cost);
        CJSON_GET_STR(maintenance_cost);
        CJSON_GET_STR(duration);
        CJSON_GET_STR(time_to_cast);
        CJSON_GET_ENUM_INT(duration_type, spell_t::duration_type_e);
        CJSON_GET_ENUM_INT(defense_type, defense_type_e);
        CJSON_GET_STR(info);
    }

    void spell_list_t::from_JSON(cJSON* root)
    {
        auto& spells = *this;
        CJSON_GET_ITEM_VECTOR(spells);
    }

    cJSON* learned_spell_t::to_JSON() const
    {
        CJSON_CREATE_ROOT();
        CJSON_ADD_ITEM(spell);
        CJSON_ADD_INT(num_improvements);
        return root;
    }

    void learned_spell_t::from_JSON(cJSON* root)
    {
        CJSON_GET_ITEM(spell);
        CJSON_GET_INT(num_improvements);
    }
}
}

CLANG_DIAGNOSTIC_POP