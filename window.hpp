#pragma once

#include <SDL.h>

class Window
{
    private:
        SDL_Window *window;

    public:
        Window(SDL_Window *w) { this->window = w; }
        ~Window(void) { SDL_DestroyWindow(this->window); }

        void init(char const*, int, int, int, int, uint32_t);
        SDL_Window *self(void) { return this->window; }
};
