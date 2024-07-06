#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <map>
#include "perlin.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SQUARE_WIDTH = 10;
const int SQUARE_HEIGHT_SEGMENTS = 10;
const int CHUNK_SIZE = 32;
const int NUM_CHUNKS = 64;
const int NUM_OCTAVES = 3;
const int NUM_SQUARES = SCREEN_WIDTH/SQUARE_WIDTH + 1;

// SDL Init
bool init(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** dirt_texture, SDL_Texture** grass_texture) {
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

    // Initialize PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    // Load texture
    *dirt_texture = IMG_LoadTexture(*renderer, "dirt.jpg");
    if (dirt_texture == NULL)
    {
        printf("Failed to load texture image! SDL_image Error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }


    *grass_texture = IMG_LoadTexture(*renderer, "grass2.png");
    if (grass_texture == NULL)
    {
        printf("Failed to load texture image! SDL_image Error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }


    return true;
}

// SDL close
void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* dirt_texture, SDL_Texture* grass_texture) {
    // Free loaded image
    SDL_DestroyTexture(dirt_texture);
    dirt_texture = NULL;

    SDL_DestroyTexture(grass_texture);
    grass_texture = NULL;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


// Actual Running of Application
int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* dirt_texture = nullptr;
    SDL_Texture* grass_texture = nullptr;

    int offset = CHUNK_SIZE*SQUARE_WIDTH;
    std::map<std::pair<int, int>, int> block_state; 

    if (argc > 1)
        offset = std::stoi(args[1]);

    if (!init(&window, &renderer, &dirt_texture, &grass_texture)) {
        std::cout << "Failed to initialize!" << std::endl;
        return -1;
    }

    int init2 = Mix_Init(8);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

    Mix_Chunk * dirt_break = Mix_LoadWAV("dirt_break.mp3");

    Perlin proc_gen (NUM_OCTAVES, NUM_CHUNKS, CHUNK_SIZE);
    bool quit = false;
    SDL_Event e;
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
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_x)
            {
                std::cout << "Chunk Number: " << 1 + (NUM_CHUNKS + offset/(SQUARE_WIDTH*CHUNK_SIZE) - 1) % NUM_CHUNKS << " out of " << NUM_CHUNKS << std::endl;
                // for (int i : heights_on_screen)
                    // std::cout << i << " ";
                // std::cout << std::endl;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int FIRST_CHUNK = (NUM_CHUNKS + offset/(SQUARE_WIDTH*CHUNK_SIZE) - 1) % NUM_CHUNKS;
                int leftpoint = offset - ((SQUARE_WIDTH*CHUNK_SIZE + offset) % (SQUARE_WIDTH*CHUNK_SIZE)) - SQUARE_WIDTH*CHUNK_SIZE;

                int blockX = (x - leftpoint + offset) / SQUARE_WIDTH;
                int blockY = (SCREEN_HEIGHT - y)/SQUARE_HEIGHT_SEGMENTS;

                if (heights_on_screen[blockX] >= blockY)
                {
                    int X = (FIRST_CHUNK*CHUNK_SIZE + blockX) % (NUM_CHUNKS*CHUNK_SIZE);
                    if (!block_state.count({X, blockY}))
                        Mix_PlayChannel(-1, dirt_break, 0);
                    block_state[{X, blockY}] = 0;
                    std::cout << X << " " << blockY << std::endl;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
        SDL_RenderClear(renderer);

        int FIRST_CHUNK = -2;
        offset = (NUM_CHUNKS*CHUNK_SIZE*SQUARE_WIDTH + offset) % (NUM_CHUNKS*CHUNK_SIZE*SQUARE_WIDTH);
        int leftpoint = offset - ((SQUARE_WIDTH*CHUNK_SIZE + offset) % (SQUARE_WIDTH*CHUNK_SIZE)) - SQUARE_WIDTH*CHUNK_SIZE;
        if (FIRST_CHUNK != (NUM_CHUNKS + offset/(SQUARE_WIDTH*CHUNK_SIZE) - 1) % NUM_CHUNKS)
        {
            FIRST_CHUNK = (NUM_CHUNKS + offset/(SQUARE_WIDTH*CHUNK_SIZE) - 1) % NUM_CHUNKS;
            heights_on_screen = proc_gen.proc_gen_heights(CHUNK_SIZE, FIRST_CHUNK, SCREEN_WIDTH/(CHUNK_SIZE*SQUARE_WIDTH) + 4);    
        }
        // std::cout << offset << " - " << leftpoint << " ";
        // for (int i : heights_on_screen)
            // std::cout << i << " ";
        // std::cout << std::endl;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
        for (int i = 0; i < heights_on_screen.size(); i++)
        {
            SDL_Rect dirt_srcRect = {0, 0, 320, 320};
            SDL_Rect destRect = {leftpoint + i * SQUARE_WIDTH - offset, SCREEN_HEIGHT - heights_on_screen[i] * SQUARE_HEIGHT_SEGMENTS, SQUARE_WIDTH, SQUARE_HEIGHT_SEGMENTS};
            // SDL_Rect destRect = {leftpoint + i * SQUARE_WIDTH - offset, SCREEN_HEIGHT - heights_on_screen[i] * SQUARE_HEIGHT_SEGMENTS, SQUARE_WIDTH, heights_on_screen[i] * SQUARE_HEIGHT_SEGMENTS};

            SDL_Rect grass_srcRect = {0, 0, 380, 380};
            if (!block_state.count({(FIRST_CHUNK*CHUNK_SIZE + i) % (NUM_CHUNKS*CHUNK_SIZE), heights_on_screen[i] - 1}))
                SDL_RenderCopy(renderer, grass_texture, &grass_srcRect, &destRect);
            
            // if (i < 4)
            // {
            //     int X = (FIRST_CHUNK*CHUNK_SIZE + i) % NUM_CHUNKS*CHUNK_SIZE;
            //     std::cout << X << " " << i << std::endl;

            // }
            for (int j = 1; j < heights_on_screen[i]; j++)
            {
                destRect.y += SQUARE_HEIGHT_SEGMENTS;
                
                if (!block_state.count({(FIRST_CHUNK*CHUNK_SIZE + i) % (NUM_CHUNKS*CHUNK_SIZE), heights_on_screen[i] - 1 - j}))
                    SDL_RenderCopy(renderer, dirt_texture, &dirt_srcRect, &destRect);
            }

            // SDL_Rect fillRect = {leftpoint + i * SQUARE_WIDTH - offset, SCREEN_HEIGHT - heights_on_screen[i] * SQUARE_HEIGHT_SEGMENTS, SQUARE_WIDTH, heights_on_screen[i] * SQUARE_HEIGHT_SEGMENTS}; // (posx, posy, width, height)
            // SDL_RenderFillRect(renderer, &fillRect);
        }
        // long square_num = offset/SQUARE_WIDTH; // identify left most square that needs to be shown
        // for (int i = square_num; i < square_num + NUM_SQUARES; ++i) { // identify the number of squares that can fit on screen and only show those
        // }

        SDL_RenderPresent(renderer);
    }

    close(window, renderer, dirt_texture, grass_texture);

    return 0;
}
