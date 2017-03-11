#ifndef SDL2_CONTEXT_HPP
#define SDL2_CONTEXT_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "sdl2_ptr.hpp"

namespace sdl2{
    class sdl2_error : public std::runtime_error{
        public:
            sdl2_error(std::string message) : runtime_error(message) {}
    };

    class sdl2_image_error : public std::runtime_error{
        public:
            sdl2_image_error(std::string message) : runtime_error(message) {}
    };

    class SDL{
        public:
            SDL(Uint32 flags=0){
                if(SDL_Init(flags) < 0){
                    std::ostringstream stream("");
                    stream << "Error Initializing SDL" << std::endl << "SDL_Error: " << SDL_GetError() << std::endl;
                    throw sdl2_error(stream.str());
                }
#ifdef DEBUG
                    std::cout << "SDL Context successfully initialized!" << std::endl;
#endif
            }
            ~SDL(){
                SDL_Quit();
#ifdef DEBUG
                std::cout << "SDL Context successfully destructed!" << std::endl;
#endif
            }
    };

    class SDL_Image{
        public:
            SDL_Image(int flags=IMG_INIT_PNG){
                if(!(IMG_Init(flags) & flags)){
                    std::ostringstream stream("");
                    stream << "Error Initializing SDL_Image" << std::endl << "IMG_Error: " << IMG_GetError() << std::endl;
                    throw sdl2_image_error(stream.str());
                } 
#ifdef DEBUG
                std::cout << "SDL_Image Context successfully initialized!" << std::endl;
#endif
            }
            ~SDL_Image(){
                IMG_Quit();
#ifdef DEBUG
                std::cout << "SDL_Image Context successfully destructed!" << std::endl;
#endif
            }
    };

    //From Eric Scott Barr's awesome article on Resource management with C++14 and SDL2
    //https://eb2.co/blog/2014/04/c-plus-plus-14-and-sdl2-managing-resources/
    template <typename Creator, typename Destructor, typename Error, typename... Arguments>
    auto make_resource(Creator c, Destructor d, Error e, std::string resource_type, Arguments&&... args){
        auto r = c(std::forward<Arguments>(args)...);
        if(!r){
            std::ostringstream stream("");
            stream << "Error while making a " << resource_type << " resource!" << std::endl << "Error: " << e() << std::endl;
            throw std::runtime_error(stream.str());
        }
        return std::unique_ptr<std::decay_t<decltype(*r)>, decltype(d)>(r,d);
    }

    template <typename Creator, typename Destructor, typename Error, typename... Arguments>
    auto make_shared_resource(Creator c, Destructor d, Error e, std::string resource_type, Arguments&&... args){
        auto r = c(std::forward<Arguments>(args)...);
        if(!r){
            std::ostringstream stream("");
            stream << "Error while making a " << resource_type << " shared resource!" << std::endl << "Error: " << e() << std::endl;
            throw std::runtime_error(stream.str());
        }
        return std::shared_ptr<std::decay_t<decltype(*r)>>(r,d);
    }

#ifdef DEBUG
    //Debug constructors and destructors
    SDL_Window* CreateWindow(const char* title, int x, int y, int w, int h, Uint32 flags){
        std::cout << "[Window+]\tCreating " << title << " window" << std::endl;
        return SDL_CreateWindow(title, x, y, w, h, flags);
    }
    
    void DestroyWindow(SDL_Window* window){
        SDL_DestroyWindow(window);
        std::cout << "[Window-]\tDestroyed window" << std::endl;
    }

    SDL_Surface* GetWindowSurface(SDL_Window* window){
        std::cout << "[Surface+]\tCreating Surface from Window" << std::endl;
        return SDL_GetWindowSurface(window);
    }

    SDL_Surface* LoadBMP(const char* file){
        std::cout << "[Surface+]\tLoading file: " << file << std::endl;
        return SDL_LoadBMP(file);
    }

    SDL_Surface* Load_Image(const char* file){
        std::cout << "[Surface+]\tLoading image: " << file << " with SDL_image" << std::endl;
        return IMG_Load(file);
    }

    SDL_Surface* ConvertSurface(SDL_Surface* surface, const SDL_PixelFormat* format, Uint32 flags = 0){
        std::cout << "[Surface+]\tConverting surface to new format" << std::endl;
        return SDL_ConvertSurface(surface, format, flags);
    }

    void FreeSurface(SDL_Surface* surface){
        SDL_FreeSurface(surface);
        std::cout << "[Surface-]\tDestroyed Surface" << std::endl;
    }

    SDL_Renderer* CreateRenderer(SDL_Window* window, int index, Uint32 flags){
        std::cout << "[Renderer+]\tCreating new Renderer" << std::endl;
        return SDL_CreateRenderer(window, index, flags);
    }

    void DestroyRenderer(SDL_Renderer* renderer){
        SDL_DestroyRenderer(renderer);
        std::cout << "[Renderer-]\tDestroying Renderer" << std::endl;
    }

    SDL_Texture* CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface){
        std::cout << "[Texture+]\tCreating Texture from surface" << std::endl;
        return SDL_CreateTextureFromSurface(renderer, surface);
    }

    void DestroyTexture(SDL_Texture* texture){
        SDL_DestroyTexture(texture);
        std::cout << "[Texture-]\tDestroying Texture" << std::endl;
    }
#endif 

    inline Window_ptr make_window(const char* title, int x, int y, int w, int h, Uint32 flags){
        return make_resource( 
#ifdef DEBUG
                CreateWindow,
                DestroyWindow,
#else
                SDL_CreateWindow,
                SDL_DestroyWindow, 
#endif
                SDL_GetError, "Window", title, x, y, w, h, flags);
    }

    inline Window_ptr make_window(){
        return Window_ptr(nullptr,
#ifdef DEBUG
                DestroyWindow
#else
                SDL_DestroyWindow
#endif
                );
    }

    inline Surface_ptr get_window_surface(SDL_Window* window){
        return make_shared_resource(
#ifdef DEBUG
                GetWindowSurface,
                FreeSurface,
#else
                SDL_GetWindowSurface,
                SDL_FreeSurface, 
#endif
                SDL_GetError, "Surface", window);
    }

    inline Surface_ptr load_bmp(const char* file){
        return make_shared_resource(
#ifdef DEBUG
                LoadBMP, 
                FreeSurface, 
#else
                IMG_Load,
                SDL_FreeSurface,
#endif
                IMG_GetError, "Loaded BMP", file);
    }

    inline Surface_ptr basic_img_load(const char* file){
        return make_shared_resource(
#ifdef DEBUG
                Load_Image,
                FreeSurface,
#else
                IMG_Load,
                SDL_FreeSurface,
#endif
                IMG_GetError, "Loaded Image", file);
    }

    inline Surface_ptr img_load(const char* file, const SDL_PixelFormat* format = nullptr){
        Surface_ptr temp = basic_img_load(file);
        if(format == nullptr){
            return temp;
        }
        else{
            return make_shared_resource(
#ifdef DEBUG
                    ConvertSurface,
                    FreeSurface,
#else
                    SDL_ConvertSurface,
                    SDL_FreeSurface,
#endif
                    SDL_GetError, "Converted Surface", temp.get(), format, 0);
        }
    }

    inline Renderer_ptr make_renderer(SDL_Window* window, int index, Uint32 flags){
        return make_resource(
#ifdef DEBUG
                CreateRenderer,
                DestroyRenderer,
#else
                SDL_CreateRenderer,
                SDL_DestroyRenderer,
#endif
                SDL_GetError, "Renderer", window, index, flags);
    }

    inline Renderer_ptr make_renderer(){
        return Renderer_ptr(nullptr,
#ifdef DEBUG
                DestroyRenderer
#else
                SDL_DestroyRenderer
#endif
                );
    }

    inline Renderer_shared make_shared_renderer(SDL_Window* window, int index, Uint32 flags){
        return make_shared_resource(
#ifdef DEBUG
                CreateRenderer,
                DestroyRenderer,
#else
                SDL_CreateRenderer,
                SDL_DestroyRenderer,
#endif
                SDL_GetError, "Shared Renderer", window, index, flags);
    }

    inline Renderer_shared make_shared_renderer(){
        return Renderer_shared(nullptr,
#ifdef DEBUG
                DestroyRenderer
#else
                SDL_DestroyRenderer
#endif
                );
    }

    inline Texture_ptr create_texture_from_surface(SDL_Renderer* renderer, SDL_Surface* surface){
        return make_shared_resource(
#ifdef DEBUG
                CreateTextureFromSurface,
                DestroyTexture,
#else
                SDL_CreateTextureFromSurface,
                SDL_DestroyTexture,
#endif
                SDL_GetError, "Surface Texture", renderer, surface);
    }

    inline Texture_ptr load_texture(SDL_Renderer* renderer, const char* file){
        Surface_ptr temp = basic_img_load(file);
        return create_texture_from_surface(renderer, temp.get());
    }

}

#endif
