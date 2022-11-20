/**
 * RENDERER:
 *  This file contains all necessary prototypes and utilities needed for
 *  Conway's Game of Life rendering functionalities
 *
 *  file: renderer.hpp
 *  author: Nathan Corcoran
 *  year: 2022
 */

#pragma once

#include <memory>

#if defined(__gnu_linux__) || defined(__linux__)
    #include <SDL2/SDL.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #include <SDL.h>
#endif

#include "window.hpp"

class Renderer
{
    private:
        SDL_Renderer *renderer;
    public:
        explicit Renderer(SDL_Renderer *r) { this->renderer = r; }
        ~Renderer(void) { SDL_DestroyRenderer(this->renderer); }

        void init(const std::shared_ptr<Window>& window, int, uint32_t);
        SDL_Renderer *self(void) { return this->renderer; }

        int set_draw_colour(uint8_t, uint8_t, uint8_t, uint8_t);
        int clear(void);
        void present(void);

        int draw_selection_box(int16_t, int16_t, uint8_t, uint8_t, uint8_t,
                uint8_t);
        int draw_filled_box(int16_t, int16_t, uint8_t, uint8_t, uint8_t,
                uint8_t);
};
