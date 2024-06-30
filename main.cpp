#include <SDL2/SDL.h>
#include <iostream>
#include <random>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SQUARE_WIDTH = 50;
const int SQUARE_HEIGHT_SEGMENTS = 30;
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
    int offset = 180;

    if (!init(&window, &renderer)) {
        std::cout << "Failed to initialize!" << std::endl;
        return -1;
    }

    bool quit = false;
    SDL_Event e;

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

        long square_num = offset/SQUARE_WIDTH; // identify left most square that needs to be shown
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
        for (int i = square_num; i < square_num + NUM_SQUARES; ++i) { // identify the number of squares that can fit on screen and only show those
            SDL_Rect fillRect = {i * SQUARE_WIDTH - offset, SCREEN_HEIGHT - i * SQUARE_HEIGHT_SEGMENTS, SQUARE_WIDTH, i * SQUARE_HEIGHT_SEGMENTS}; // (posx, posy, width, height)
            SDL_RenderFillRect(renderer, &fillRect);
        }

        SDL_RenderPresent(renderer);
    }

    close(window, renderer);

    return 0;
}
