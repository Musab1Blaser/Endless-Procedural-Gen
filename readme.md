# Endless Procedural Generation
This is a small side project of mine with the goal of learning more about random numbers and procedural world generation.

## How it works
The viewport position is stored. Only chunks that have coordinates that lie within/near the viewport are generated to save memory. If you leave a chunk and come back to it later, it will still be the same chunk despite regeneration because chunks are generated through a random number and the random number for each chunk is recovered/recalculated at the time of generating the chunk. The user effects on the terrain are maintained by storing them in a dictionary and applying them on the terrain after it is generated.

## Project Overview Video:


## How to use
Compilation command (Linux) - requires SDL2, SDL_image and SDL_mixer: <br>
```g++ main.cpp perlin.cpp -lSDL2 -lSDL2_image -lSDL2_mixer -o "ProcGen.out" -Wall```

Run: <br>
```./ProcGen.out``` <br>
This will load a new world seeded based on user cpu time. You can navigate the map by scrolling. Blocks can be broken by clicking on them. Press 'x' to see which chunk you are currently viewing and 'z' to output program memory usage.