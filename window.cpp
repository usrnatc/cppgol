#include <SDL.h>

#include "window.hpp"

void
Window::init(char const* window_title, int window_x, int window_y,
        int window_width, int window_height, uint32_t window_flags)
{
    this->window = SDL_CreateWindow(window_title, window_x, window_y,
            window_width, window_height, window_flags);
}