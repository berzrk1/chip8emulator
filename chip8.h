#ifndef CHIP8_H
#define CHIP8_H

typedef struct Chip8 chip8;

chip8 *initialize();
void cleanup(chip8 *chip);
int load_game(chip8 *chip, char *gamefile);
void emulateCycle(chip8 *chip);

#endif // !CHIP8_H
