#include <iostream>
#include <cmath>
#include <utility>
#include <thread>
#include <mutex>
#include <SDL.h>

#include "game.hpp"
#include "window.hpp"
#include "renderer.hpp"

using namespace std;

Game::Game(void)
{
    this->g_window = make_shared<Window>(nullptr);
    this->g_renderer = make_shared<Renderer>(nullptr);
    this->g_board = new uint8_t[G_BOARD_SIZE * G_BOARD_SIZE]();
    this->g_state = GAME_RUNNING;
    this->g_paused = false;
}

Game::~Game()
{
    this->g_renderer.reset();   /* destroys renderer? */
    this->g_window.reset();     /* destroys window? */
    delete[] this->g_board;
    SDL_Quit();
}

int
Game::init(int unsigned window_width, int unsigned window_height)
{
    int rc;

    if (rc = SDL_Init(SDL_INIT_EVERYTHING), rc) {

        cerr << "[ERROR] :: holy fuck everything died" << endl;
        goto out;
    }

    this->g_window->init("Conway's Game of Life",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width,
            window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!this->g_window->self()) {

        cerr << "[ERROR] :: g_window" << endl;
        rc = EXIT_FAILURE;
        goto out;
    }

    this->g_renderer->init(this->g_window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!this->g_renderer->self()) {

        cerr << "[ERROR] :: g_renderer" << endl;
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

void
Game::set_cell(int x, int y, uint8_t val)
{
    int b_x = (x < 0 || x >= G_BOARD_SIZE) ? calculate_circular_index(x) : x;
    int b_y = (y < 0 || y >= G_BOARD_SIZE) ? calculate_circular_index(y) : y;

    if (this->g_board[b_x + b_y * G_BOARD_SIZE] != val)
        this->g_board[b_x + b_y * G_BOARD_SIZE] = val;
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
    int         b_x;
    int         b_y;
    uint32_t    m_btns;

    m_btns = SDL_GetMouseState(&m_x, &m_y);
    if (m_x && m_y) {

        /* round value to nearest multiple of 10 */
        b_x = m_x / 10;
        b_y = m_y / 10;

        this->renderer()->draw_selection_box(b_x * 10, b_y * 10, 255, 0, 0, 255);

        if (m_btns & SDL_BUTTON_LMASK)
            this->set_cell(b_x, b_y, 1);
        else if (m_btns & SDL_BUTTON_RMASK)
            this->set_cell(b_x, b_y, 0);
    }
}

void
Game::clear_board(void)
{
    uint8_t *empty_board = new uint8_t[G_BOARD_SIZE * G_BOARD_SIZE]();

    delete[] this->g_board;
    this->g_board = empty_board;
}

void
Game::next_iteration(void)
{
    uint8_t *next_iteration = new uint8_t[G_BOARD_SIZE * G_BOARD_SIZE]();

    for (int y = 0; y < G_BOARD_SIZE; y++) {

        for (int x = 0; x < G_BOARD_SIZE; x++) {

            int neighbour_count = this->calculate_neighbour_count(x, y);
            uint8_t current_cell = this->get_cell(x, y);

            next_iteration[x + y * G_BOARD_SIZE] = current_cell;
            if (current_cell) {

                if (neighbour_count < 2 || neighbour_count > 3)
                    next_iteration[x + y * G_BOARD_SIZE] = 0;
            } else {

                if (neighbour_count == 3)
                    next_iteration[x + y * G_BOARD_SIZE] = 1;
            }
        }
    }

    delete[] this->g_board;
    this->g_board = next_iteration;
}

void
Game::display_board(void)
{
    for (int y = 0; y < G_BOARD_SIZE; y++) {

        for (int x = 0; x < G_BOARD_SIZE; x++) {

            if (this->get_cell(x, y)) {

                this->renderer()->draw_filled_box(x * 10, y * 10, 255, 255, 255, 255);
            }
        }
    }
}

void
Game::loop(void)
{
    SDL_Event event;
    int unsigned previous_tick = 0;
    int unsigned current_tick;

    while (this->g_state == GAME_RUNNING) {
        
        current_tick = SDL_GetTicks();
        this->g_renderer->clear();
        this->handle_mouse();

        if (!this->g_paused) {

            if ((current_tick - previous_tick) > 1000 / 5.0) {
            
                this->next_iteration();
                previous_tick = current_tick;
            }
        }
        this->display_board();

        while (SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_QUIT:
                    this->g_state = GAME_STOPPED;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {

                        case SDLK_p:
                            this->g_paused = !this->g_paused;
                            break;
                        case SDLK_c:
                            this->clear_board();
                    }
            }
        }
        this->g_renderer->present();
    }
}
