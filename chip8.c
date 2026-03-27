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

chip8 *initialize() {
    // Initialize registers and memory once
    chip8 *chip = malloc(sizeof(chip8));
    chip->memory = calloc(4096, sizeof(uint8_t));
    chip->PC = 0x200;                            // Program starts at 0x200
    chip->opcode = 0;                            // Reset current opcode
    chip->I = 0;                                 // Reset address register
    chip->sp = 0;                                // Reset stack pointer
    memset(chip->stack, 0, sizeof(chip->stack)); // Clear stack
    memset(chip->gfx, 0, sizeof(chip->gfx));     // Clear display
    memset(chip->V, 0, sizeof(chip->V));         // Clear registers
    printf("Chip8 initialized");

    // TODO: Load fonts
    // TODO: Reset timers
    return chip;
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

    printf("ROM loaded");
    return 0;
}

void cleanup(chip8 *chip) {
    // Clean up
    free(chip->memory);
    free(chip);
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
        // TODO: Implement opcodes (1/35)

    case 0x0000:
        switch (chip->opcode & 0x000F) {
        case 0x0000: // 00E0: Clear screen
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x000E: // 00EE: Returns from a subroutine
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
            chip->PC += 2;
        }
        break;
    case 0x1000: // 1NNN: Jumps to address NNN
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0x2000: // 2NNNN: Calls subroutine at NNN
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0x3000: // 3XNN: Skips the next instruction if VX equals NN
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0x4000: // 4XNN: Skips the next instruction if VX does not equals NN
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0x5000: // 5XY0: Skips the next instruction if VX equals VY
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0x6000: // 6XNN: Sets VX to NN
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0x7000: // 7XNN: Adds NN to VX
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0x8000:
        switch (chip->opcode & 0x000F) {
        case 0x0000: // 8XY0: Sets VX to the value of VY
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        case 0x0001:
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        case 0x0002:
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        case 0x0003:
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        case 0x0004:
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        case 0x0005:
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        case 0x0006:
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        case 0x0007:
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        case 0x000E:
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;
        default:
            printf("Unknown opcode [0x8000]: 0x%X\n", chip->opcode);
            chip->PC += 2;
        }
        break;

    case 0x9000: // 9XY0: Skips the next instruction if VX does not equal VY
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0xA000: // ANNN: Sets I to the address NNN
        chip->I = chip->opcode & 0x0FFF;
        chip->PC += 2; // Skip 2 since we read 2 memory address
        break;

    case 0xB000: // BNNN: Jumps to the address NNN plus V0
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0xC000: // CXNN:
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0xD000:
        printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
        chip->PC += 2;
        break;

    case 0xE000:
        switch (chip->opcode & 0x000F) {
        case 0x000E: // EX9E
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x0001: // EXA1
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", chip->opcode);
            chip->PC += 2;
        }
        break;

    case 0xF000:
        switch (chip->opcode & 0x00FF) {
        case 0x0007: // FX07
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x000A: // FX0A
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x0015: // FX15
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x0018: // FX18
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x001E: // FX1E
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x0029: // FX29
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x0033: // FX33
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x0055: // FX55
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        case 0x0065: // FX65
            printf("Opcode (0x%X) not implemented yet\n", chip->opcode);
            chip->PC += 2;
            break;

        default:
            printf("Unknown opcode [0xF000]: 0x%X\n", chip->opcode);
            chip->PC += 2;
        }
        break;

    default:
        printf("Unknown opcode: 0x%X\n", chip->opcode);
        chip->PC += 2;
        break;
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
