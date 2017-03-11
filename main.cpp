//STD Headers
#include <iostream>
#include <memory>
#include <tuple>

//SDL_Headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define DEBUG

//Local Headers
#include "sdl2_context.hpp"
#include "component_manager.hpp"
#include "components.hpp"
#include "render_system.hpp"
#include "entities.hpp"
#include "asset_manager.hpp"


//Screen dimensionn constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

using game_components = component_pack<
    render_component, 
    sprite_component, 
    size_component, 
    position_component
>;

int main(int, char*){

    //Start up SDL and create window
    sdl2::SDL sdl_context(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    sdl2::SDL_Image sdl_image_context;

    //Create Window
    sdl2::Window_ptr main_window = sdl2::make_window("SDL Tutorial",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    //Create Window Texture Renderer
    sdl2::Renderer_shared renderer = sdl2::make_shared_renderer( main_window.get(), -1, SDL_RENDERER_ACCELERATED);

    //Define the Component Manager
    component_manager<game_components> comp_manager;

    //Define the Asset Manager
    asset_manager::asset_manager assets(renderer);

    /***************/
    /*Setup Systems*/
    /***************/

    //Initialize the Render System
    render_system<game_components> render_system{comp_manager, assets, std::move(main_window), renderer};
    render_system.initialize();

    //Register media
    assets.register_sprite("background", "Assets/loaded.png", nullptr, {"background", "level1"}, false);

    //Load media
    assets.load_sprite("background");

    //create_image
    entity::create_image(comp_manager, "background", 10, 10, SCREEN_WIDTH-20, SCREEN_HEIGHT-20, true);

    bool quit = false;
    SDL_Event e;
    while(!quit){
        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT){
                quit = true;
            }
        }
        render_system.update();
    }

    return 0;
}
