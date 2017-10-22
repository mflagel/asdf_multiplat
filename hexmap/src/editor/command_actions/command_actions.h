#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>

#include <asdf_multiplat/utilities/utilities.h>
#include <glm/glm.hpp>

#include "data/hex_map.h"

namespace asdf {
namespace hexmap {
namespace editor
{
    using tile_coord_dict_t = std::unordered_map<glm::ivec2, asdf::hexmap::data::hex_tile_id_t>;
    
    struct editor_t;

    //undoable action
    struct editor_action_t
    {
        editor_action_t() = default;
        virtual ~editor_action_t() = default;

        virtual void execute() = 0;
        virtual void unexecute() = 0;
    };

    struct action_stack_t
    {
        editor_t& editor;
        std::vector<std::unique_ptr<editor_action_t>> actions;
        std::vector<std::unique_ptr<editor_action_t>> undone_actions;

        action_stack_t(editor_t&);

        void undo();
        void redo();

        bool can_undo() const;
        bool can_redo() const;

        void clear();

        void push(std::unique_ptr<editor_action_t>&&);
        void push_and_execute(std::unique_ptr<editor_action_t>&&);

        // template<typename T, typename ...Args>
        // void push(Args&& ...args)
        // {
        //     push( std::make_unique<T>(std::forward<Args>(args)...) );
        // }

        void pop();
        void pop_and_unexecute();
    };


    /// Action Subclasses
    struct paint_tiles_action_t : editor_action_t
    {
        data::hex_grid_t& hex_grid;
        tile_coord_dict_t painted_tiles;
        uint32_t new_tile_id = 0;

        paint_tiles_action_t(data::hex_grid_t&, tile_coord_dict_t painted_tiles, uint32_t new_tile_id);

        void execute() override;
        void unexecute() override;
    };

    struct add_map_object_action_t : editor_action_t
    {
        data::hex_map_t& map_data;
        data::map_object_t obj;

        add_map_object_action_t(data::hex_map_t&, data::map_object_t);

        void execute() override;
        void unexecute() override;
    };

    struct modify_map_objects_action_t : editor_action_t
    {
        data::map_object_t& obj;
        data::map_object_t old_state;

        modify_map_objects_action_t(data::map_object_t&, data::map_object_t old_state);

        void execute() override;
        void unexecute() override;
    };

    struct delete_map_objects_action_t : editor_action_t
    {
        data::hex_map_t& map_data;
        std::unordered_map<data::object_index_t, data::map_object_t> old_objects;

        delete_map_objects_action_t(data::hex_map_t&, data::object_index_t old_index);
        // delete_map_objects_action_t(data::hex_map_t&, std::vector<data::object_index_t> old_indices);

        template <typename L>
        delete_map_objects_action_t(data::hex_map_t& _map_data, L _old_indices)
        : map_data(_map_data)
        {
            // static_assert(std::is_same_v<L::value_type, data::object_index_t>
            //             , "_old_indices must contain objects of type 'data::object_index_t'");

            for(auto const& obj_ind : _old_indices)
            {
                old_objects.insert({obj_ind, map_data.objects[obj_ind]});
            }
        }

        void execute() override;
        void unexecute() override;
    };

    struct add_spline_action_t : editor_action_t
    {
        data::hex_map_t& map_data;
        data::spline_t spline;

        add_spline_action_t(data::hex_map_t&, data::spline_t const&);

        void execute() override;
        void unexecute() override;
    };

    // struct resize_grid_action_t : editor_action_t
    // {
    //     data::hex_grid_t& hex_grid;
    //     glm::ivec2 prev_size;
    //     glm::ivec2 new_size;

    //     //std::vector<__something__> chopped_data; 

    //     void execute() override;
    //     void unexecute() override;
    // };

}
}
}