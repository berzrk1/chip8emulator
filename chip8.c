#include "chip8.h"
#include <stdint.h>
#include <stdlib.h>

struct Chip8 {
    uint8_t *memory;    // addresses
    uint8_t V[16];      // registers
    uint16_t I;         // address register
    uint16_t PC;        // program counter
    uint16_t stack[16]; // stack
    uint8_t sp;         // stack pointer
    bool gfx[64 * 32];  // display
    uint16_t opcode;
    uint8_t delay_timer;
    uint8_t sound_timer;
};

void initialize(chip8 *chip) {
    // Initialize registers and memory once
    chip->memory = malloc(4096);
}

void cleanup(chip8 *chip) {
    // Clean up
    free(chip->memory);
}

void emulateCycle(chip8 *chip) {
    // Simulate one cycle of the Chip 8
    // 1.Fetch Opcode
    // 2.Decode Opcode
    // 3.Excute Opcode
}
