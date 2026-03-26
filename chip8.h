#ifndef CHIP8_H
#define CHIP8_H

typedef struct Chip8 chip8;

void initialize(chip8 *chip);
void cleanup(chip8 *chip);
void emulateCycle(chip8 *chip);

#endif // !CHIP8_H
