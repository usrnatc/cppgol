#include <iostream>
#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

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
