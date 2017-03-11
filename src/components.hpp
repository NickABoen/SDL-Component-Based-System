#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <memory>
#include <utility>

#include <SDL2/SDL.h>

#include "game_components.hpp"
#include "sdl2_ptr.hpp"

/************************************************/
/*             Renderable Component             */
/************************************************/
struct render_component : public game_component{
    bool is_visible;

    render_component(component_id id, bool is_visible=true):
        game_component(id),
        is_visible(is_visible)
    {}
};

/************************************************/
/*               Sprite Component               */
/************************************************/
struct sprite_component : public game_component{
    std::string sprite_name;

    sprite_component(component_id id, std::string sprite_name):
        game_component(id),
        sprite_name(sprite_name)
    {}
};

/************************************************/
/*                Size Component                */
/************************************************/
struct size_component : public game_component{
    float width;
    float height;

    size_component(component_id id, float w, float h):
        game_component(id),
        width(w),
        height(h)
    {}
    size_component(component_id id, int w, float h) : size_component(id, (float)w, (float)h){}
};

/************************************************/
/*              Position Component              */
/************************************************/
struct position_component : public game_component{
    float x;
    float y;

    position_component(component_id id, float x, float y)   : game_component(id), x(x), y(y) {}
    position_component(component_id id, int x, int y)       : position_component(id, (float)x, (float)y){}
    position_component(component_id id, SDL_Point point)    : position_component(id, point.x, point.y){}
};

#endif
