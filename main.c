#include "chip8.h"
#include <stdint.h>

int main(int argc, char **argv) {
    uint8_t memory[4096];
    uint16_t opcode;
    uint8_t V[16];     // registers
    uint16_t I;        // address register
    uint16_t PC;       // program counter
    bool gfx[64 * 32]; // display
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t stack[16];
    uint8_t sp;

    // Load Game File from argument

    // Emulation loop
    for (;;) {
        // 1. Emulate Cycle
        // 2. Update screen, if draw flag is set
        // 3. Store key press state
    }

    return 0;
}
