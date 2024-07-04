#include <SDL2/SDL.h>
#include <iostream>
#include <random>
#include "perlin.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SQUARE_WIDTH = 10;
const int SQUARE_HEIGHT_SEGMENTS = 10;
const int CHUNK_SIZE = 16;
const int NUM_CHUNKS = 64;
const int NUM_OCTAVES = 3;
const int NUM_SQUARES = SCREEN_WIDTH/SQUARE_WIDTH + 1;

// SDL Init
bool init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(*window);
        return false;
    }

    return true;
}

// SDL close
void close(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


// Actual Running of Application
int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int offset = 0;

    if (!init(&window, &renderer)) {
        std::cout << "Failed to initialize!" << std::endl;
        return -1;
    }

    Perlin proc_gen (NUM_OCTAVES, NUM_CHUNKS, CHUNK_SIZE);
    bool quit = false;
    SDL_Event e;
    int prev_offset = -1;
    std::vector<int> heights_on_screen;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEWHEEL) {
                if (e.wheel.y > 0) { // Scroll up - screen left
                    offset -= 10;
                } else if (e.wheel.y < 0) { // Scroll down - screen right
                    offset += 10;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
        SDL_RenderClear(renderer);

        int FIRST_CHUNK = (NUM_CHUNKS + offset/(SQUARE_WIDTH*CHUNK_SIZE) - 1) % NUM_CHUNKS;
        int leftpoint = offset - ((SQUARE_WIDTH*CHUNK_SIZE + offset) % (SQUARE_WIDTH*CHUNK_SIZE)) - SQUARE_WIDTH*CHUNK_SIZE;
        if (offset != prev_offset)
            heights_on_screen = proc_gen.proc_gen_heights(CHUNK_SIZE, (NUM_CHUNKS + offset/(SQUARE_WIDTH*CHUNK_SIZE) - 1) % NUM_CHUNKS, SCREEN_WIDTH/(CHUNK_SIZE*SQUARE_WIDTH) + 4);    
        prev_offset = offset;
        std::cout << offset;
        // for (int i : heights_on_screen)
            // std::cout << i << " ";
        // std::cout << std::endl;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
        for (int i = 0; i < heights_on_screen.size(); i++)
        {
            SDL_Rect fillRect = {leftpoint + i * SQUARE_WIDTH, SCREEN_HEIGHT - heights_on_screen[i] * SQUARE_HEIGHT_SEGMENTS, SQUARE_WIDTH, heights_on_screen[i] * SQUARE_HEIGHT_SEGMENTS}; // (posx, posy, width, height)
            SDL_RenderFillRect(renderer, &fillRect);
        }
        // long square_num = offset/SQUARE_WIDTH; // identify left most square that needs to be shown
        // for (int i = square_num; i < square_num + NUM_SQUARES; ++i) { // identify the number of squares that can fit on screen and only show those
        // }

        SDL_RenderPresent(renderer);
    }

    close(window, renderer);

    return 0;
}
