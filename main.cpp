/**
 * CGOL:
 *  This is a simple implementation of Conway's Game of Life
 *
 *  The program consists of a Window, a Renderer, and finally the Game logic
 *  as per standard SDL2 programs
 */

#include <memory>

#if defined(__gnu_linux__) || defined(__linux__)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL2_gfxPrimitives.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #include <SDL.h>
    #include <SDL_ttf.h>
    #include <SDL2_gfxPrimitives.h>
#endif

#include "game.hpp"

int
main([[maybe_unused]] int argv, [[maybe_unused]] char **args)
{
    std::unique_ptr<Game> game = std::make_unique<Game>();
    int rc = EXIT_SUCCESS;

    rc = game->init(800, 800);
    if (rc)
        goto out;

    game->loop();

out:
    return (rc);
}
