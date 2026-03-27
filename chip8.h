#ifndef CHIP8_H
#define CHIP8_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct Chip8 chip8;

chip8 *initialize();
void cleanup(chip8 *chip);
int load_game(chip8 *chip, char *gamefile);
void emulateCycle(chip8 *chip);
void drawGrapics(chip8 *chip, SDL_Renderer *renderer);
void handleInput(chip8 *chip, SDL_Event *event);
void updateTimers(chip8 *chip);

#endif // !CHIP8_H
