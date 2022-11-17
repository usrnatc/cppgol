#include <iostream>
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
#include "window.hpp"
#include "renderer.hpp"

using namespace std;

int
main(int argv, char **args)
{
    unique_ptr<Game> game = make_unique<Game>();
    int rc = EXIT_SUCCESS;

    rc = game->init(800, 800);
    if (rc)
        return (rc);

    game->loop();

    return (rc);
}
