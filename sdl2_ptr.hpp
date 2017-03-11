#ifndef SDL2_PTR_HPP
#define SDL2_PTR_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace sdl2{
    using Window_ptr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
    using Renderer_ptr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
    using Renderer_shared = std::shared_ptr<SDL_Renderer>;
    using Surface_ptr = std::shared_ptr<SDL_Surface>;
    using Texture_ptr = std::shared_ptr<SDL_Texture>;
}

#endif
