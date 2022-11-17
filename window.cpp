/**
 * WINDOW:
 *  This file contains all necessary functionality to create an SDL window
 *
 *  file: window.cpp
 *  author: Nathan Corcoran
 *  year: 2022
 */

#if defined(__gnu_linux__) || defined(__linux__)
    #include <SDL2/SDL.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #include <SDL.h>
#endif

#include "window.hpp"

void
Window::init(char const* window_title, int window_x, int window_y,
        int window_width, int window_height, uint32_t window_flags)
{
    this->window = SDL_CreateWindow(window_title, window_x, window_y,
            window_width, window_height, window_flags);
}
