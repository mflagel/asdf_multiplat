#include "stdafx.h"
#include "character.h"

using namespace std;
using namespace glm;

#include "cJSON/cJSON.h"

namespace gurps_track
{
namespace data
{
    // using namespace basic_info;

    bool trait_t::is_advantage() const
    {
        return point_cost > 0;
    }
    bool trait_t::is_disadvantage() const
    {
        return point_cost < -1;
    }
    bool trait_t::is_quirk() const
    {
        return point_cost == -1 || point_cost == 0;
    }

    int race_t::point_cost() const
    {
        int total = 0;
        for(auto const& trait : traits)
        {
            total += trait.point_cost;
        }

        return total;
    }

    char_point_transaction_t::char_point_transaction_t(int _amount, std::string _description, date_t _date)
    : amount(_amount)
    , description(_description)
    , date(_date)
    {
    }

    character_info_t::character_info_t()
    {
    }

    character_t::character_t(std::string filepath)
    : character_info_t()
    , character_filepath(filepath)
    {
        // try
        // {
        //     load_from_file(filepath);
        // }
        // catch (...)
        {
            load_default_data();
            // load_test_data();
        }
    }

    void character_t::load_default_data()
    {
        character_filepath = "default.grp";

        base_stats[stat_ST] = 10;
        base_stats[stat_DX] = 10;
        base_stats[stat_IQ] = 10;
        base_stats[stat_HT] = 10;

        int current_fatigue =  10;
        int current_HP = 10;

        race.name = "Human";
        race.description = "Human by Default";

        cache_mods();
        derive_stats();

        point_transactions.empty();
        unspent_points = 0;
        gain_points(100, "initial normal point budget");
    }

    void character_t::load_test_data()
    {
        name = "Das Test";
        description = "I may or may not be a test";
        portrait_filepath = "";

        gain_points(64, "points to test with");

        base_stats[stat_ST] = 11;
        base_stats[stat_DX] = 14;
        base_stats[stat_IQ] = 12;
        base_stats[stat_HT] = 10;

        race.name = "Generic Race";
        race.description = "This is really just a test race, nothing actually from the book\n Testing new lines in descriptions";

        //test race trait
        trait_t test_race_trait;
        test_race_trait.name = "test race trait 1";
        test_race_trait.description = "description for test race trait 1";
        test_race_trait.point_cost = 10;
        test_race_trait.status_effects.push_back(status_effect_t{"test effect", effect_base_stat, stat_ST, 2});
        race.traits.push_back(move(test_race_trait));


        //test advantage
        trait_t test_advantage;
        test_advantage.name = "test advantage 1";
        test_advantage.description = "description for test advantage 1";
        test_advantage.point_cost = 25;
        // test_advantage.status_effects.push_back(status_effect_t{"test effect", effect_base_stat, stat_magery, 2});
        traits.push_back(test_advantage);

        //test disadvantage
        trait_t test_disadvantage;
        test_disadvantage.name = "test disadvantage 1";
        test_disadvantage.description = "description for test disadvantage 1";
        test_disadvantage.point_cost = 25;
        test_disadvantage.status_effects.push_back(status_effect_t{"test effect", effect_derived_stat, stat_willpower, -2});
        traits.push_back(test_disadvantage);

        trait_t test_quirk;
        test_quirk.name = "test quirk 1";
        test_quirk.description = "description for test quirk 1";
        test_quirk.point_cost = -1;
        traits.push_back(test_quirk);

        trait_t test_quirk_2;
        test_quirk_2.name = "test quirk 2";
        test_quirk_2.description = "description for test quirk 2";
        test_quirk_2.point_cost = -1;
        traits.push_back(test_quirk_2);


        cache_mods();
        derive_stats();

        current_HP = derived_stats[stat_HP];
        current_fatigue = derived_stats[stat_fatigue];
    }

    void character_t::derive_stats()
    {
        derived_stats[stat_HP]      = HT();
        derived_stats[stat_fatigue] = ST();

        // stat_basic_speed = ((DX() + HT()) * 100) / 4.0f;
        derived_stats[stat_movement]    = std::floor( (DX() + HT()) / 4.0f );

        auto iq = IQ();
        derived_stats[stat_willpower] = iq;
        derived_stats[stat_vision]    = iq;
        derived_stats[stat_hearing]   = iq;
        derived_stats[stat_smell]     = iq;
        derived_stats[stat_taste]     = iq;
        derived_stats[stat_touch]     = iq;
    }

    void character_t::cache_mods()
    {
        for(auto& mod : stat_mod_cache)
        {
            mod = 0;
        }

        for(auto const& trait : race.traits)
        {
            for(auto const& effect : trait.status_effects)
            {
                cache_effect(effect);
            }
        }

        for(auto const& trait : traits)
        {
            for(auto const& effect : trait.status_effects)
            {
                cache_effect(effect);
            }
        }
    }

    void character_t::add_effect(status_effect_t effect)
    {
        status_effects.push_back(effect);
        cache_effect(effect);
    }

    void character_t::cache_effect(const status_effect_t effect)
    {
        size_t i = effect_type_indexes[effect.effect_type] + effect.type_index;
        stat_mod_cache[i] += effect.amount;
    }

    // bool character_t::remove_effect(const status_effect_t)
    // {
    //        
    // }
    // bool character_t::remove_effect(status_effect_t*)
    // {
    //
    // }
    void character_t::remove_effect(size_t index)
    {
        ASSERT(index < status_effects.size(), "");

        //un-cache mod
        auto const& effect = status_effects[index];
        size_t i = effect_type_indexes[effect.effect_type] + effect.type_index;
        stat_mod_cache[i] -= effect.amount;

        status_effects.erase(status_effects.begin() + index);
    }


    int character_t::get_stat(base_stat_e stat) const
    {
        return base_stats[stat] + get_cached_mod(stat);
    }
    int character_t::get_stat(derived_stat_e stat) const
    {
        return derived_stats[stat] + stat_mod_cache[base_stat_count + stat];
    }

    int character_t::get_cached_mod(base_stat_e stat) const
    {
        return stat_mod_cache[stat];
    }
    int character_t::get_cached_mod(derived_stat_e stat) const
    {
        return stat_mod_cache[stat + base_stat_count];
    }


    int character_t::PD(armor_region_e region) const
    {
        auto info = armor_info[region];
        return gurps_track::data::armor_info[info.type].PD + info.PD_mod;
    }
    int character_t::DR(armor_region_e region) const
    {
        auto info = armor_info[region];
        return gurps_track::data::armor_info[info.type].DR + info.DR_mod;
    }

    int character_t::stats_point_cost() const
    {
        int stats_cost = 0;
        for(size_t stat_index = size_t(stat_ST); stat_index <= size_t(stat_HT); ++stat_index)
        {
            stats_cost += get_stat_point_cost(base_stats[stat_index]);
        }

        return stats_cost;
    }
    int character_t::advantages_point_cost() const
    {
        int adv_total = 0;
        for(auto const& trait : traits)
        {
            if(trait.is_advantage())
            {
                adv_total += trait.point_cost;
            }
        }

        return adv_total;
    }
    int character_t::disadvantages_point_cost() const
    {
        int disadv_total = 0;
        for(auto const& trait : traits)
        {
            if(trait.is_disadvantage())
            {
                disadv_total += trait.point_cost;
            }
        }

        return disadv_total;
    }
    int character_t::quirks_point_cost() const
    {
        int quirk_total = 0;
        for(auto const& trait : traits)
        {
            if(trait.is_quirk())
            {
                quirk_total += trait.point_cost;
            }
        }

        return quirk_total;
    }
    int character_t::traits_point_cost() const
    {
        int total = 0;
        for(auto const& trait : traits)
        {
            total += trait.point_cost;
        }

        return total;
    }
    double character_t::skills_point_cost() const
    {
        double skill_total = 0.0;
        for(auto const& skill : skills)
        {
            //0.5 point value is just stored as 0 so I can use an int
            skill_total += (skill.num_improvements == 0) ? 0.5 : skill.point_cost();
        }

        return skill_total;
    }
    int character_t::spells_point_cost() const
    {
        int spell_total = 0; //spells can't have half-points
        for(auto const& spell : spells)
        {
            spell_total += spell.point_cost(); //no need to handle half-points for spells
        }

        return spell_total;
    }
    double character_t::total_point_cost() const
    {
        return stats_point_cost()
             + race.point_cost()
             + traits_point_cost()
             + skills_point_cost()
             + spells_point_cost();
    }


    void character_t::spend_points(uint16 amount, std::string memo)
    {
        points_transaction(-amount, move(memo));
    }

    void character_t::gain_points(uint16 amount, std::string memo)
    {
        points_transaction(amount, move(memo));
    }

    void character_t::points_transaction(int amount, std::string memo)
    {
        LOG("point transaction: %i, %s", amount, memo.c_str());
        point_transactions.push_back( char_point_transaction_t(amount, move(memo)) );
        unspent_points += amount;
        LOG("unspent points remaining: %i", unspent_points);
    }


    void character_t::heal_damage(int amount_healed)
    {
        current_HP = std::min(current_HP + amount_healed, max_HP());
    }

    void character_t::take_damage(int damage_taken, damage_type_e damage_type, hit_location_e hit_location)
    {
        auto armor = armor_info[armor_region_from_hit_location(hit_location)];
        current_HP -= calc_damage(damage_taken, damage_type, armor.type, armor.DR_mod);
    }

    bool character_t::has_skill(std::string const& skill_name) const
    {
        for(auto listing : skills)
        {
            if(listing.skill.name == skill_name)
                return true;
        }

        return false;
    }

    void character_t::learn_skill(skill_t skill, int initial_points)
    {
        ASSERT(!has_skill(skill.name), "Skill already learned");

        LOG("Skill added: %s (%s) at %i points", skill.name.c_str(), skill_difficulty_abbreviations[skill.difficulty], initial_points);
        skills.push_back(learned_skill_t(std::move(skill), initial_points));
    }

    // void character_t::upgrade_skill(std::string const& skill_name)
    // {
    //     ASSERT(has_skill(skill_name), "Cannot upgrade non-learned skill");
    // }

    bool character_t::can_improve_skill(size_t listing_index/*, int amount*/) const
    {
        ASSERT(listing_index < skills.size(), "");
        return unspent_points >= skills[listing_index].improvement_cost(/*amount*/);
    }

    bool character_t::improve_skill(size_t listing_index)//, int amount)
    {
        ASSERT(listing_index < skills.size(), "");
        // LOG_IF(amount == 0, "improving skill by zero for some reason");

        auto& listing = skills[listing_index];

        // if(amount > 0)
        {
            // if(can_improve_skill(listing_index))  //ignore point cost checking until transaction system is finished
            {
                string memo = "Improved \'" + listing.skill.name + "\' from " + to_string(listing.num_improvements) + " to " + to_string(listing.num_improvements + 1);
                LOG("%s", memo.c_str());

                spend_points(listing.improvement_cost(), move(memo));
                ++listing.num_improvements;

                return true;
            }
            // else
            // {
            //     LOG("Not enough points to improve %s from %i to %i", listing.skill.name.c_str(), listing.num_improvements, listing.num_improvements + 1 );

            //     return false;
            // }
        }
        // else
        // {
        //     LOG_IF(amount < 0 && listing.num_improvements == 0, "cannot further reduce %s, already at half point");
        //     LOG_IF(amount < 0 && abs(amount) > listing.num_improvements, "de-improve amount would put skill below zero improvement; capping at 0 (half-point)");

        //     amount = std::max(amount, -listing.num_improvements);

        //     // gain_points(

        //     return false;
        // }
    }


    void character_t::save_to_file(std::string filepath)
    {
        cJSON* root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "character_info", to_JSON());

        std::string json_str = cJSON_Print(root);
        cJSON_Delete(root);

        /// FIXME: Add util stuff from asdf_multiplat
        // util::write_text_file(filepath, json_str);
    }

    void character_t::save_data()
    {
        if(character_filepath.size() == 0)
        {
            character_filepath = name + ".grp";
        }

        save_to_file(character_filepath);
    }


    void character_t::load_from_file(std::string filepath)
    {
        /// FIXME: Add util stuff from asdf_multiplat
        // std::string json_str = util::read_text_file(filepath);
        // cJSON* root = cJSON_Parse(json_str.c_str());

        // ASSERT(root, "");
        // ASSERT(cJSON_GetObjectItem(root, "character_info"), "");
        // from_JSON(cJSON_GetObjectItem(root, "character_info"));
        // cJSON_Delete(root);

        // cache_mods();
        // derive_stats();

        // unspent_points = 0;
        // for(auto const& transaction : point_transactions)
        // {
        //     unspent_points += transaction.amount;
        // }
    }
}
}