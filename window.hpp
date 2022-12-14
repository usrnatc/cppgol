/**
 * WINDOW:
 *  This file contains all necessary prototype and utilities needed for
 *  an SDL window
 *
 *  file: window.hpp
 *  author: Nathan Corcoran
 *  year: 2022
 */

#pragma once

#if defined(__gnu_linux__) || defined(__linux__)
    #include <SDL2/SDL.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #include <SDL.h>
#endif

class Window
{
    private:
        SDL_Window *window;

    public:
        explicit Window(SDL_Window *w) { this->window = w; }
        ~Window(void) { SDL_DestroyWindow(this->window); }

        void init(char const*, int, int, int, int, uint32_t);
        SDL_Window *self(void) { return this->window; }
};
