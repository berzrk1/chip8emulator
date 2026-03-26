#include "chip8.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    chip->memory = calloc(4096, sizeof(uint8_t));
    chip->PC = 0x200;                            // Program starts at 0x200
    chip->opcode = 0;                            // Reset current opcode
    chip->I = 0;                                 // Reset address register
    chip->sp = 0;                                // Reset stack pointer
    memset(chip->stack, 0, sizeof(chip->stack)); // Clear stack
    memset(chip->gfx, 0, sizeof(chip->gfx));     // Clear display
    memset(chip->V, 0, sizeof(chip->V));         // Clear registers

    // TODO: Load fonts
    // TODO: Reset timers
}

int load_game(chip8 *chip, char *gamefile) {
    // Load game
    FILE *fp = fopen(gamefile, "rb");

    if (!fp) {
        perror("fopen");
        return -1;
    }

    // Set pointer to end of file to get size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp); // go back to the start of the file

    // Load the game into the memory
    size_t bytes_loaded = fread(chip->memory + 0x200, 1, (size_t)size, fp);
    fclose(fp);

    if (bytes_loaded != (size_t)size) {
        fprintf(stderr, "failed to read entire ROM\n");
        return -1;
    }

    return 0;
}

void cleanup(chip8 *chip) {
    // Clean up
    free(chip->memory);
}

void emulateCycle(chip8 *chip) {
    // Simulate one cycle of the Chip 8

    // Fetch Opcode
    // Since opcode is 2 bytes long, we need to
    // fetch two successive bytes and merge them
    chip->opcode =
        (uint16_t)(chip->memory[chip->PC] << 8 | chip->memory[chip->PC + 1]);

    // Decode Opcode
    // Read the first byte to get the opcode
    switch (chip->opcode & 0xF000) {
        // TODO: Implement opcodes

    case 0xA000: // ANNN: Sets I to the address NNN
        chip->I = chip->opcode & 0x0FFF;
        chip->PC += 2; // Skip 2 since we read 2 memory address
        break;

    default:
        printf("Unknown opcode: 0x%X\n", chip->opcode);
    }

    if (chip->delay_timer > 0)
        chip->delay_timer--;

    if (chip->sound_timer > 0) {
        if (chip->sound_timer == 1) {
            printf("BEEP\n");
        }
        chip->sound_timer--;
    }
}
