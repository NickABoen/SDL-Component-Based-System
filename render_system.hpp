#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

//STL headers

//SDL2 headers
#include <SDL2/SDL.h>

//Local headers
#include "sdl2_context.hpp"
#include "base_system.hpp"
#include "components.hpp"
#include "asset_manager.hpp"

template <class ComponentPack>
class render_system : public base_system<ComponentPack>, public system_interface{
    private:
        sdl2::Window_ptr window;
        sdl2::Renderer_shared renderer;
        asset_manager::asset_manager& assets;

        void draw(sdl2::Texture_ptr texture,
                SDL_Rect* clip_rect = nullptr,
                SDL_Rect* dst_rect = nullptr);
    public:
        void initialize();
        void update();

        render_system(component_manager<ComponentPack>& component_pools, asset_manager::asset_manager& assets) :
            render_system(component_pools, assets,
                    sdl2::make_window(), 
                    sdl2::make_shared_renderer())
        {}
        render_system(component_manager<ComponentPack>& component_pools, asset_manager::asset_manager& assets,
                sdl2::Window_ptr&& window) :
            render_system(component_pools, assets,
                    std::move(window), 
                    sdl2::make_shared_renderer())
        {}
        render_system(component_manager<ComponentPack>& component_pools, asset_manager::asset_manager& assets,
                sdl2::Window_ptr&& window, 
                sdl2::Renderer_shared renderer):
            ::base_system<ComponentPack>(component_pools), 
            window(std::move(window)), 
            renderer(renderer),
            assets(assets)
        {}
};

template <class ComponentPack>
void render_system<ComponentPack>::initialize(){
    SDL_SetRenderDrawColor(renderer.get(), 0x00, 0x00, 0x00, 0x00);
}

template <class ComponentPack>
void render_system<ComponentPack>::update(){
    if(!this->is_enabled) return;
    if(!window || !renderer){
        std::cout << "Warning - Invalid ";
        if(!window && !renderer) std::cout << "Window and Renderer";
        else if(!window) std::cout << "Window";
        else if(!renderer) std::cout << "Renderer";
        std::cout << std::endl;
    }

    SDL_RenderClear(renderer.get());

    //auto component_pools = base_system<ComponentPack>::component_pools;

    auto& render_pool = base_system<ComponentPack>::component_pools.template get<render_component>();
    auto& sprite_pool = base_system<ComponentPack>::component_pools.template get<sprite_component>();
    auto& size_pool = base_system<ComponentPack>::component_pools.template get<size_component>();
    auto& position_pool = base_system<ComponentPack>::component_pools.template get<position_component>();

    for(auto& value : render_pool){
        auto id = value.first;
        auto& render_component = value.second;
        auto contains_id = make_contains(id);

        if(render_component.is_visible && contains_id(sprite_pool)){
            auto& sprite_component = sprite_pool.at(id);
            auto sprite_asset = assets.get_sprite(sprite_component.sprite_name);
            //Trying to reliably construct an SDL_Rect* and pass it back
            std::unique_ptr<SDL_Rect> form_rect = nullptr;
            auto has_size = contains_id(size_pool);
            auto has_position = contains_id(position_pool);
            if(has_size || has_position){
                float x = 0.0f, y = 0.0f, w = 1.0f, h = 1.0f;
                if(has_size){
                    auto& size_component = size_pool.at(id);
                    w = size_component.width;
                    h = size_component.height;
                }
                if(has_position){
                    auto& position_component = position_pool.at(id);
                    x = position_component.x;
                    y = position_component.y;
                }
                form_rect = std::make_unique<SDL_Rect>(SDL_Rect{x,y,w,h});
            }

            if(SDL_RenderCopy(
                        renderer.get(), 
                        sprite_asset.texture.get(), 
                        sprite_asset.clipping_rect.get(), 
                        form_rect.get())){
                std::cout << "Error while rendering texture." << std::endl
                    << "Error: " << SDL_GetError() << std::endl;
            }
        }
    }

    SDL_RenderPresent(renderer.get());
}

#endif
