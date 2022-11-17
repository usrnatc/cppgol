#include <memory>

#if defined(__gnu_linux__) || defined(__linux__)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL2_gfxPrimitives.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #include <SDL.h>
    #include <SDL2_gfxPrimitives.h>
#endif

#include "window.hpp"
#include "renderer.hpp"

void
Renderer::init(std::shared_ptr<Window> window, int renderer_index,
        uint32_t renderer_flags)
{
    this->renderer = SDL_CreateRenderer(window->self(), renderer_index,
            renderer_flags);
}

int
Renderer::set_draw_colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    int rc;

    rc = SDL_SetRenderDrawColor(this->renderer, r, g, b, a);
    return (rc);
}

int
Renderer::clear(void)
{
    int rc;
    uint8_t *r = new uint8_t,
            *g = new uint8_t,
            *b = new uint8_t,
            *a = new uint8_t;

    /* store previous draw colour for 'context-switching' */
    rc = SDL_GetRenderDrawColor(this->renderer, r, g, b, a);
    if (rc) {

        /* if we cant determine previous draw colour, forget about it */
        fprintf(stderr,
                "[INFO] :: %s :: could not determine previous draw colour\n",
                __func__);
        *r = 0;
        *g = 0;
        *b = 0;
        *a = 255;
    }
    this->set_draw_colour(0, 0, 0, 255);
    rc = SDL_RenderClear(this->renderer);
    /* re-instate previous draw colour */
    this->set_draw_colour(*r, *g, *b, *a);

    delete r;
    delete g;
    delete b;
    delete a;

    return (rc);
}

int
Renderer::draw_selection_box(int16_t x, int16_t y, uint8_t r, uint8_t g,
        uint8_t b, uint8_t a)
{
    int rc;

    rc = roundedRectangleRGBA(this->renderer, x, y, x + 10, y + 10, 2,
            r, g, b, a);
    return (rc);
}

int
Renderer::draw_filled_box(int16_t x, int16_t y, uint8_t r, uint8_t g,
        uint8_t b, uint8_t a)
{
    int rc;

    rc = boxRGBA(this->renderer, x, y, x + 10, y + 10, r, g, b, a);
    return (rc);
}

void
Renderer::present(void)
{
    SDL_RenderPresent(this->renderer);
}
