#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <memory>
#include <tuple>
#include <utility>

#include "game_components.hpp"
#include "component_manager.hpp"

namespace entity{
    static id_generator generator = id_generator();

    component_id generate_id(){ return generator();}

    template <class ComponentType>
    component_id create_image(
            component_manager<ComponentType>& manager, 
            std::string sprite_name, 
            int x, int y, int width, int height, bool is_visible){

        component_id id = generate_id();

        auto& render_pool = manager.template get<render_component>();
        auto& sprite_pool = manager.template get<sprite_component>();
        auto& size_pool = manager.template get<size_component>();
        auto& position_pool = manager.template get<position_component>();

        auto pool_emplace = make_emplace_id(id);

        pool_emplace(render_pool, is_visible);
        pool_emplace(sprite_pool, sprite_name);
        pool_emplace(size_pool, width, height);
        pool_emplace(position_pool, x, y);
    }
}

#endif
