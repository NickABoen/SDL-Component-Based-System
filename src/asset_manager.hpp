#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <map>
#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <SDL2/SDL.h>

#include "game_components.hpp"
#include "sdl2_context.hpp"
#include "component_manager.hpp"

namespace asset_manager{
    /******************************************************************************/
    /*                          Asset Manager Components                          */
    /******************************************************************************/
    using asset_id = component_id;
    using sprite_id = component_id;

    struct asset_component : game_component{
        std::string filename;
        std::vector<std::string> tags;
        bool is_loaded;
        bool load_on_demand;

        asset_component(
                asset_id id,
                std::string filename, 
                std::vector<std::string>& tags,
                bool load_on_demand = false) : 
            game_component(id),
            filename(filename), 
            tags(tags), 
            is_loaded(false), 
            load_on_demand(load_on_demand)
        {}
    };

    struct texture_component : game_component{
        sdl2::Texture_ptr texture;

        texture_component(asset_id id, sdl2::Texture_ptr texture) : 
            game_component(id), texture(texture){}
    };

    struct sprite_component : game_component{
        std::string name;
        std::unique_ptr<SDL_Rect> clipping_rect;
        asset_id texture_id;

        sprite_component(sprite_id id, 
                std::string name, 
                std::unique_ptr<SDL_Rect>&& clipping_rect,
                asset_id texture_id) :
            game_component(id),
            name(name), 
            clipping_rect(std::move(clipping_rect)),
            texture_id(texture_id)
        {}
    };


    /******************************************************************************/
    /*                      Asset Manager Component Manager                       */
    /******************************************************************************/
    using asset_manager_components = component_pack<
        asset_component,
        texture_component,
        sprite_component
    >;
    using asset_manager_component_manager = component_manager<asset_manager_components>;


    /******************************************************************************/
    /*                          Asset Manager Declarations                        */
    /******************************************************************************/
    using asset_index = std::map<std::string, asset_id>;
    using sprite_index = std::map<std::string, sprite_id>;

    struct sprite_asset{
        sdl2::Texture_ptr texture;
        std::unique_ptr<SDL_Rect> clipping_rect;

        sprite_asset(sdl2::Texture_ptr texture, std::unique_ptr<SDL_Rect> clipping_rect):
            texture(texture), clipping_rect(std::move(clipping_rect)){}
    };

    class asset_manager{
        private:
            asset_manager_component_manager component_maps;
            sdl2::Renderer_shared renderer;
            id_generator generator;
            asset_index assets;
            sprite_index sprites;

            component_id generate_id(){return generator();}
            void load_asset(std::string filename);
            void load_texture(asset_id id);
            void load_image_tags(
                    std::vector<std::string>& tags_to_load, 
                    bool unload_remaining = false);
            void unload_asset(std::string filename);
            void unload_texture(asset_id id);
            bool has_matching_tags(
                    std::vector<std::string>& first_tag_set, 
                    std::vector<std::string>& second_tag_set);

        public:
            asset_manager(sdl2::Renderer_shared renderer) :
                component_maps(), renderer(renderer), generator(), assets(), sprites(){}
            void register_sprite(
                    std::string sprite_name, 
                    std::string filename,
                    std::unique_ptr<SDL_Rect> clipping_rect = nullptr,
                    std::vector<std::string> tags = {},
                    bool load_on_demand = false);

            void load_sprite(std::string sprite_name);

            void load_asset_tags(
                    std::vector<std::string> tags_to_load, 
                    bool unload_remaining = false);
            sprite_asset get_sprite(std::string sprite_name);
            void unload_all();
    };

    /******************************************************************************/
    /*                         Asset Manager Definitions                          */
    /******************************************************************************/
    void asset_manager::register_sprite(
            std::string sprite_name, 
            std::string filename,
            std::unique_ptr<SDL_Rect> clipping_rect, 
            std::vector<std::string> tags, 
            bool load_on_demand){

        auto contains_asset = make_contains(filename);
        auto contains_sprite = make_contains(sprite_name);

        bool asset_exists = contains_asset(assets);
        bool sprite_exists = contains_sprite(sprites);

        if(sprite_exists){
            std::cout << "Warning - sprite name \"" 
                << sprite_name << "\" already exists" << std::endl;
            return;
        }

        auto& asset_pool = component_maps.template get<asset_component>();
        auto& texture_pool = component_maps.template get<texture_component>();

        //It's alright if many sprites may reference the same asset so
        //it's not a big deal if it already exists, but the asset
        //itself needs to be created if it doesn't already exist
        if(!asset_exists){
            asset_id id_of_asset = generate_id();
            auto pool_emplace_asset = make_emplace_id(id_of_asset);
            pool_emplace_asset(asset_pool, filename, tags, load_on_demand);
            pool_emplace_asset(texture_pool, nullptr);
            assets[filename] = id_of_asset;
        }

        //Asset is guaranteed to exist after this point

        asset_id id_of_asset = assets.at(filename);
        sprite_id id_of_sprite = generate_id();

        auto& sprite_pool = component_maps.template get<sprite_component>();
        auto pool_emplace_sprite = make_emplace_id(id_of_sprite);

        pool_emplace_sprite(sprite_pool, sprite_name, std::move(clipping_rect), id_of_asset);
        sprites[sprite_name] = id_of_sprite;
    }

    void asset_manager::load_asset_tags(
            std::vector<std::string> tags_to_load,
            bool unload_remaining){
        load_image_tags(tags_to_load, unload_remaining);
    }

    void asset_manager::load_image_tags(
            std::vector<std::string>& tags_to_load, 
            bool unload_remaining){

        for(auto& asset_pair : assets){
            auto& asset_pool = component_maps.template get<asset_component>();
            auto id_of_asset = asset_pair.second;
            auto& asset = asset_pool.at(id_of_asset);
            if(!asset.is_loaded && has_matching_tags(tags_to_load, asset.tags)){
                load_asset(asset.filename);
            }
            else if(unload_remaining){
                unload_asset(asset.filename);
            }
        }
    }

    void asset_manager::unload_asset(std::string filename){
        auto id_of_asset = assets.at(filename);
        
        unload_texture(id_of_asset);
    }

    void asset_manager::unload_texture(asset_id id){
        auto& asset_pool = component_maps.template get<asset_component>();
        auto& asset = asset_pool.at(id);
        if(asset.is_loaded){
            auto& texture_pool = component_maps.template get<texture_component>();
            auto& texture = texture_pool.at(id);
            texture.texture = nullptr;
            asset.is_loaded = false;
        }
    }

    bool asset_manager::has_matching_tags(
            std::vector<std::string>& first_tag_set, std::vector<std::string>& second_tag_set){
        return (find_first_of(std::begin(first_tag_set), std::end(first_tag_set), 
                    std::begin(second_tag_set), std::end(second_tag_set)) 
                != std::end(first_tag_set));
    }

    void asset_manager::load_sprite(std::string sprite_name){
        auto id_of_sprite = sprites.at(sprite_name);
        auto& sprite_pool = component_maps.template get<sprite_component>();
        auto& sprite = sprite_pool.at(id_of_sprite);
        load_texture(sprite.texture_id);
    }

    void asset_manager::load_asset(std::string filename){
        auto id_of_asset = assets.at(filename);
        auto& asset_pool = component_maps.template get<asset_component>();
        auto asset = asset_pool.at(id_of_asset);

        if(!asset.is_loaded){
            auto contains_id = make_contains(id_of_asset);
            auto& texture_pool = component_maps.template get<sprite_component>();
            if(contains_id(texture_pool)){
                load_texture(id_of_asset);
            }
            else{
                std::cout << "Warning - Asset type of filename[" 
                    << asset.filename << "] cannot be determined!" << std::endl;
            }
        }
    }

    void asset_manager::load_texture(asset_id id){
        auto& asset_pool = component_maps.template get<asset_component>();
        auto& texture_pool = component_maps.template get<texture_component>();

        auto& asset = asset_pool.at(id);
        auto& texture = texture_pool.at(id);

        if(!asset.is_loaded || texture.texture == nullptr){
            texture.texture = sdl2::load_texture(renderer.get(), asset.filename.c_str());
            asset.is_loaded = true;
        }
    }

    sprite_asset asset_manager::get_sprite(std::string sprite_name){
        auto id_of_sprite = sprites.at(sprite_name);

        auto& sprite_pool = component_maps.template get<sprite_component>();
        auto& asset_pool = component_maps.template get<asset_component>();
        auto& texture_pool = component_maps.template get<texture_component>();

        auto& sprite = sprite_pool.at(id_of_sprite);
        auto id_of_texture = sprite.texture_id;
        auto& texture = texture_pool.at(id_of_texture);
        auto& asset = asset_pool.at(id_of_texture);

        if(!asset.load_on_demand && texture.texture == nullptr){
            std::cout << "Warning - texture for sprite_name[" << sprite_name << "] is not loaded correctly" << std::endl;
        }
        else if(asset.load_on_demand){
            load_texture(id_of_texture);
        }
        std::unique_ptr<SDL_Rect> rect_ptr = sprite.clipping_rect ? 
                std::make_unique<SDL_Rect>(*(sprite.clipping_rect.get())) :
                nullptr;

        return sprite_asset(texture.texture, std::move(rect_ptr));
    }

    void asset_manager::unload_all(){
        auto& asset_pool = component_maps.template get<asset_component>();
        auto& texture_pool = component_maps.template get<texture_component>();

        for(auto& texture_pair : texture_pool){
            auto& texture = texture_pair.second;
            texture.texture = nullptr;
        }

        for(auto& asset_pair : asset_pool){
            auto& asset = asset_pair.second;
            asset.is_loaded = false;
        }
    }
}

#endif
