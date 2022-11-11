#pragma once

#include <memory>
#include <SDL.h>

#include "window.hpp"
#include "renderer.hpp"

class Game
{
    private:
        std::shared_ptr<Window> g_window;
        std::shared_ptr<Renderer> g_renderer;
        uint8_t *g_board;

        enum State {
            GAME_RUNNING,
            GAME_STOPPED,
        };

        void handle_mouse(void);
        void set_cell(int, int, uint8_t);
        uint8_t get_cell(int, int) const;
        int calculate_neighbour_count(int, int) const;
        void display_board(void);
        void next_iteration(void);
        void clear_board(void);

    public:
        Game(void);
        ~Game(void);

        State g_state;
        bool g_paused;

        std::shared_ptr<Window> window(void) { return this->g_window; }
        std::shared_ptr<Renderer> renderer(void) { return this->g_renderer; }

        int init(int unsigned, int unsigned);
        void loop(void);
#define G_BOARD_SIZE 80
};
