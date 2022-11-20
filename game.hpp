/**
 * GAME:
 *  This file contains all prototypes and utilities needed for the
 *  game logic of Conway's Game of Life
 *
 *  file: game.hpp
 *  author: Nathan Corcoran
 *  year: 2022
 */

#pragma once

#include <memory>
#include <vector>
#include <cstdint>

#if defined(__gnu__linux__) || defined(__linux__)
    #include <SDL2/SDL.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #include <SDL.h>
#endif

#include "window.hpp"
#include "renderer.hpp"

class Game
{
    private:
        std::shared_ptr<Window> g_window;
        std::shared_ptr<Renderer> g_renderer;
        std::vector<uint8_t> g_board;
        std::vector<uint8_t> g_next_iteration;
        int g_brush;

        enum State {
            G_RUNNING,
            G_STOPPED,
        };

        std::vector<void (Game::*)(int, int)> const g_brush_selections = {
            &Game::select_cell,
            &Game::select_block,
            &Game::select_beehive,
            &Game::select_loaf,
            &Game::select_boat,
            &Game::select_tub,
            &Game::select_blinker,
            &Game::select_toad,
            &Game::select_beacon,
            &Game::select_glider,
        };

        std::vector<void (Game::*)(int, int, int)> const g_brush_placements = {
            &Game::place_cell,
            &Game::place_block,
            &Game::place_beehive,
            &Game::place_loaf,
            &Game::place_boat,
            &Game::place_tub,
            &Game::place_blinker,
            &Game::place_toad,
            &Game::place_beacon,
            &Game::place_glider,
        };

        void handle_mouse(void);
        void handle_keyboard(SDL_Event *, int *);
        void set_cell(int, int, uint8_t);
        uint8_t get_cell(int, int) const;
        int calculate_neighbour_count(int, int) const;
        void display_board(void);
        void next_iteration(void);
        void clear_board(void);
        void next_brush(int);

        void place_cell(int, int, int);
        void place_block(int, int, int);
        void place_beehive(int, int, int);
        void place_loaf(int, int, int);
        void place_boat(int, int, int);
        void place_tub(int, int, int);
        void place_blinker(int, int, int);
        void place_toad(int, int, int);
        void place_beacon(int, int, int);
        void place_glider(int, int, int);

        void select_cell(int, int);
        void select_block(int, int);
        void select_beehive(int, int);
        void select_loaf(int, int);
        void select_boat(int, int);
        void select_tub(int, int);
        void select_blinker(int, int);
        void select_toad(int, int);
        void select_beacon(int, int);
        void select_glider(int, int);

    public:
#define G_BOARD_SIZE 80
        Game(void);
        ~Game(void);

        State g_state;
        bool g_paused;

        std::shared_ptr<Window> window(void) { return this->g_window; }
        std::shared_ptr<Renderer> renderer(void) { return this->g_renderer; }

        int init(int unsigned, int unsigned);
        void loop(void);

};
