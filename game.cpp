/**
 * GAME:
 *  This file contains all game logic necessary for Conway's Game of Life
 *
 *  file: game.cpp
 *  author: Nathan Corcoran
 *  year: 2022
 */

#include <cstdio>
#include <cstdint>
#include <algorithm>

#if defined(__gnu_linux__) || defined(__linux__)
    #include <SDL2/SDL.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #include <SDL.h>
#endif

#include "game.hpp"
#include "window.hpp"
#include "renderer.hpp"

Game::Game(void)
{
    this->g_window = std::make_shared<Window>(nullptr);
    this->g_renderer = std::make_shared<Renderer>(nullptr);
    this->g_board = std::vector<uint8_t>(G_BOARD_SIZE * G_BOARD_SIZE);
    this->g_next_iteration = std::vector<uint8_t>(G_BOARD_SIZE * G_BOARD_SIZE);
    this->g_state = G_RUNNING;
    this->g_brush = 0;
    this->g_paused = false;
}

Game::~Game()
{
    this->g_renderer.reset();   /* destroys renderer? */
    this->g_window.reset();     /* destroys window? */
    this->g_board.clear();
    SDL_Quit();
}

int
Game::init(int unsigned window_width, int unsigned window_height)
{
    int rc;

    if (rc = SDL_Init(SDL_INIT_EVERYTHING), rc) {

        fprintf(stderr, "[ERROR] :: %s :: SDL_Init\n", __func__);
        goto out;
    }

    this->g_window->init("Conway's Game of Life",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width,
            window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!this->g_window->self()) {

        fprintf(stderr, "[ERROR] :: %s :: g_window\n", __func__);
        rc = EXIT_FAILURE;
        goto out;
    }

    this->g_renderer->init(this->g_window, -1,
                           SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!this->g_renderer->self()) {

        fprintf(stderr, "[ERROR] :: %s :: g_renderer\n", __func__);
        rc = EXIT_FAILURE;
    }

out:
    return (rc);
}

int
calculate_circular_index(int a)
{
    int mod = a % G_BOARD_SIZE;
    return (mod < 0) ? (mod + G_BOARD_SIZE) : (mod);
}

int
framerate_bounds_check(int frame_delim, int delta)
{
    frame_delim += delta;

    if (frame_delim > 60)
        frame_delim = 60;
    if (frame_delim < 1)
        frame_delim = 1;

    return (frame_delim);
}

void
Game::set_cell(int x, int y, uint8_t val)
{
    int b_x = (x < 0 || x >= G_BOARD_SIZE) ? calculate_circular_index(x) : x;
    int b_y = (y < 0 || y >= G_BOARD_SIZE) ? calculate_circular_index(y) : y;

    if (this->g_board[b_x + b_y * G_BOARD_SIZE] != val) {
        this->g_board[b_x + b_y * G_BOARD_SIZE] = val;
    }

    if (val)
        this->remember_cell(b_x, b_y);
    else
        this->forget_cell(b_x, b_y);
}

uint8_t
Game::get_cell(int x, int y) const
{
    int b_x = (x < 0 || x >= G_BOARD_SIZE) ? calculate_circular_index(x) : x;
    int b_y = (y < 0 || y >= G_BOARD_SIZE) ? calculate_circular_index(y) : y;

    return this->g_board[b_x + b_y * G_BOARD_SIZE];
}

int
Game::calculate_neighbour_count(int x, int y) const
{
    int neighbour_count = 0;

    neighbour_count += this->get_cell(x - 1, y - 1);
    neighbour_count += this->get_cell(x, y - 1);
    neighbour_count += this->get_cell(x + 1, y - 1);
    neighbour_count += this->get_cell(x - 1, y);
    neighbour_count += this->get_cell(x + 1, y);
    neighbour_count += this->get_cell(x - 1, y + 1);
    neighbour_count += this->get_cell(x, y + 1);
    neighbour_count += this->get_cell(x + 1, y + 1);

    return (neighbour_count);
}

void
Game::handle_mouse(void)
{
    int         m_x;
    int         m_y;
    uint32_t    m_btns;

    m_btns = SDL_GetMouseState(&m_x, &m_y);
    if (m_x && m_y) {

        int b_x;
        int b_y;

        /* round value to the nearest multiple of 10 */
        b_x = m_x / 10;
        b_y = m_y / 10;

        (this->*(this->g_brush_selections[this->g_brush]))(b_x, b_y);

        if (m_btns & SDL_BUTTON_LMASK)
            (this->*(this->g_brush_placements[this->g_brush]))(b_x, b_y, 1);
        else if (m_btns & SDL_BUTTON_RMASK)
            (this->*(this->g_brush_placements[this->g_brush]))(b_x, b_y, 0);
    }
}

void
Game::handle_keyboard(SDL_Event *event, int *frame_delim)
{
    while (SDL_PollEvent(event)) {

        switch (event->type) {

            case SDL_QUIT:
                this->g_state = G_STOPPED;
                break;
            case SDL_KEYDOWN:
                switch (event->key.keysym.sym) {

                    case SDLK_p:
                        this->g_paused = !this->g_paused;
                        break;
                    case SDLK_c:
                        this->clear_board();
                        break;
                    case SDLK_RIGHT:
                        this->next_brush(1);
                        break;
                    case SDLK_LEFT:
                        this->next_brush(-1);
                        break;
                }
                break;
            case SDL_MOUSEWHEEL:
                if (event->wheel.y > 0)
                    *frame_delim = framerate_bounds_check(*frame_delim, 1);
                else if (event->wheel.y < 0)
                    *frame_delim = framerate_bounds_check(*frame_delim, -1);
                break;
        }
    }
}

void
Game::clear_board(void)
{
    std::vector<uint8_t>::iterator iter;
    for (iter = this->g_board.begin(); iter != this->g_board.end(); iter++) {

        *iter = 0;
    }
    this->g_alive_cells.clear();
}

void
Game::next_iteration(void)
{
    for (int y = 0; y < G_BOARD_SIZE; y++) {

        for (int x = 0; x < G_BOARD_SIZE; x++) {

            int neighbour_count = this->calculate_neighbour_count(x, y);
            uint8_t current_cell = this->get_cell(x, y);

            this->g_next_iteration[x + y * G_BOARD_SIZE] = current_cell;
            // at any given point in the simulation, it is more likely that 
            // there will be more dead cells than alive cells on the board
            if (current_cell) [[unlikely]] {

                if (neighbour_count < 2 || neighbour_count > 3) {
                    this->g_next_iteration[x + y * G_BOARD_SIZE] = 0;
                    this->forget_cell(x, y);
                }
            } else [[likely]] {

                if (neighbour_count == 3) {
                    this->g_next_iteration[x + y * G_BOARD_SIZE] = 1;
                    this->remember_cell(x, y);
                }
            }
        }
    }

    std::swap(this->g_board, this->g_next_iteration);
}

void
Game::display_board(void)
{
    for (auto& [x, y] : this->g_alive_cells) {

        this->renderer()->draw_filled_box(x * 10, y * 10, 255, 255, 255, 255);
    }
}

void
Game::loop(void)
{
    SDL_Event       event;
    int unsigned    previous_tick = 0;
    int unsigned    current_tick;
    int             frame_delim = 5;
    int             err;

    while (this->g_state == G_RUNNING) {

        if (err = SDL_ShowCursor(SDL_DISABLE), err < 0)
            this->g_state = G_STOPPED;

        current_tick = SDL_GetTicks();
        this->g_renderer->clear();
        this->handle_keyboard(&event, &frame_delim);

        if (!this->g_paused) {

            if ((current_tick - previous_tick) > 1000 / frame_delim) {

                this->next_iteration();
                previous_tick = current_tick;
            }
        }
        this->display_board();
        this->handle_mouse();
        this->g_renderer->present();
    }
}

void
Game::next_brush(int delta)
{
    int size = static_cast<int>(this->g_brush_selections.size());
    this->g_brush += delta;
    if (this->g_brush >= size)
        this->g_brush = 0;
    if (this->g_brush < 0)
        this->g_brush = size - 1;
}

void
Game::remember_cell(int x, int y)
{
    for (const auto& [x_alive, y_alive]: this->g_alive_cells) {

        if (x == x_alive && y == y_alive)
            return;
    }
    std::pair<int, int> cell_memory = {x, y};
    this->g_alive_cells.emplace_back(cell_memory);
}

void
Game::forget_cell(int x, int y)
{
    std::pair<int, int> cell_memory = {x, y};
    this->g_alive_cells.erase(std::remove(this->g_alive_cells.begin(),
                this->g_alive_cells.end(), cell_memory),
            this->g_alive_cells.end());
}

void
Game::select_cell(int x, int y)
{
    this->renderer()->draw_selection_box(x * 10, y * 10, 255, 0, 0, 255);
}

void
Game::select_block(int x, int y)
{
    this->renderer()->draw_selection_box(x * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y + 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, (y + 1) * 10, 255, 0, 0, 255);
}

void
Game::select_beehive(int x, int y)
{
    this->renderer()->draw_selection_box((x - 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y + 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, (y + 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 2) * 10, y * 10, 255, 0, 0, 255);
}

void
Game::select_loaf(int x, int y)
{
    this->renderer()->draw_selection_box((x - 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y + 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, (y + 2) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 2) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 2) * 10, (y + 1) * 10, 255, 0, 0, 255);
}

void
Game::select_boat(int x, int y)
{
    this->renderer()->draw_selection_box((x - 1) * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x - 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y + 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, y * 10, 255, 0, 0, 255);
}

void
Game::select_tub(int x, int y)
{
    this->renderer()->draw_selection_box((x - 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y + 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, y * 10, 255, 0, 0, 255);
}

void
Game::select_blinker(int x, int y)
{
    this->renderer()->draw_selection_box(x * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y + 1) * 10, 255, 0, 0, 255);
}

void
Game::select_toad(int x, int y)
{
    this->renderer()->draw_selection_box((x - 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 2) * 10, (y - 1) * 10, 255, 0, 0, 255);
}

void
Game::select_beacon(int x, int y)
{
    this->renderer()->draw_selection_box((x - 1) * 10, (y - 2) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x - 1) * 10, (y - 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y - 2) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, (y + 1) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 2) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 2) * 10, (y + 1) * 10, 255, 0, 0, 255);
}

void
Game::select_glider(int x, int y)
{
    this->renderer()->draw_selection_box((x - 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box(x * 10, (y - 2) * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, y * 10, 255, 0, 0, 255);
    this->renderer()->draw_selection_box((x + 1) * 10, (y - 1) * 10, 255, 0, 0, 255);
}

void
Game::place_cell(int x, int y, int val)
{
    this->set_cell(x, y, val);
}

void
Game::place_block(int x, int y, int val)
{
    this->set_cell(x, y, val);
    this->set_cell(x, y + 1, val);
    this->set_cell(x + 1, y, val);
    this->set_cell(x + 1, y + 1, val);
}

void
Game::place_beehive(int x, int y, int val)
{
    this->set_cell(x - 1, y, val);
    this->set_cell(x, y - 1, val);
    this->set_cell(x, y + 1, val);
    this->set_cell(x + 1, y - 1, val);
    this->set_cell(x + 1, y + 1, val);
    this->set_cell(x + 2, y, val);
}

void
Game::place_loaf(int x, int y, int val)
{
    this->set_cell(x - 1, y, val);
    this->set_cell(x, y - 1, val);
    this->set_cell(x, y + 1, val);
    this->set_cell(x + 1, y - 1, val);
    this->set_cell(x + 1, y + 2, val);
    this->set_cell(x + 2, y, val);
    this->set_cell(x + 2, y + 1, val);
}

void
Game::place_boat(int x, int y, int val)
{
    this->set_cell(x - 1, y - 1, val);
    this->set_cell(x - 1, y, val);
    this->set_cell(x, y - 1, val);
    this->set_cell(x, y + 1, val);
    this->set_cell(x + 1, y, val);
}

void
Game::place_tub(int x, int y, int val)
{
    this->set_cell(x - 1, y, val);
    this->set_cell(x, y - 1, val);
    this->set_cell(x, y + 1, val);
    this->set_cell(x + 1, y, val);
}

void
Game::place_blinker(int x, int y, int val)
{
    this->set_cell(x, y - 1, val);
    this->set_cell(x, y, val);
    this->set_cell(x, y + 1, val);
}

void
Game::place_toad(int x, int y, int val)
{
    this->set_cell(x - 1, y, val);
    this->set_cell(x, y - 1, val);
    this->set_cell(x, y, val);
    this->set_cell(x + 1, y - 1, val);
    this->set_cell(x + 1, y, val);
    this->set_cell(x + 2, y - 1, val);
}

void
Game::place_beacon(int x, int y, int val)
{
    this->set_cell(x - 1, y - 2, val);
    this->set_cell(x - 1, y - 1, val);
    this->set_cell(x, y - 2, val);
    this->set_cell(x + 1, y + 1, val);
    this->set_cell(x + 2, y, val);
    this->set_cell(x + 2, y + 1, val);
}

void
Game::place_glider(int x, int y, int val)
{
    this->set_cell(x - 1, y, val);
    this->set_cell(x, y, val);
    this->set_cell(x, y - 2, val);
    this->set_cell(x + 1, y, val);
    this->set_cell(x + 1, y - 1, val);
}
