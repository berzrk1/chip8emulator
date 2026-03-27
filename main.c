#include "chip8.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Usage: chip8emu <gamefile>");
        return EXIT_SUCCESS;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Chip-8 Emulator", WINDOW_WIDTH,
                                     WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("CreateWindowAndRenderer failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    chip8 *chip = initialize();
    // Load Game File from argument
    load_game(chip, argv[1]);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                    running = false;
            default:
                handleInput(chip, &event);
                break;
            }
        }

        // ~500Hz CPU / 60Hz display = ~8 cycles per frame
        for (int i = 0; i < 9; i++) {
            emulateCycle(chip);
        }
        updateTimers(chip);

        drawGrapics(chip, renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(160);
    }

    cleanup(chip);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
