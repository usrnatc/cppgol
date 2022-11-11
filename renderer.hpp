#pragma once

#include <memory>
#include <SDL.h>

#include "window.hpp"

class Renderer
{
    private:
        SDL_Renderer *renderer;
    public:
        Renderer(SDL_Renderer *r) { this->renderer = r; }
        ~Renderer(void) { SDL_DestroyRenderer(this->renderer); }

        void init(std::shared_ptr<Window> window, int, uint32_t);
        SDL_Renderer *self(void) { return this->renderer; }

        int set_draw_colour(uint8_t, uint8_t, uint8_t, uint8_t);
        int clear(void);
        void present(void);

        int draw_filled_circle(int16_t, int16_t, int16_t, uint8_t, uint8_t,
                uint8_t, uint8_t);
        int draw_selection_box(int16_t, int16_t, uint8_t, uint8_t, uint8_t,
                uint8_t);
        int draw_filled_box(int16_t, int16_t, uint8_t, uint8_t, uint8_t,
                uint8_t);
};
