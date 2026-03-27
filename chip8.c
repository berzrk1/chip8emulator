#include "chip8.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Chip8 {
    uint8_t *memory;    // addresses
    uint8_t V[16];      // registers
    uint16_t I;         // address register
    uint16_t PC;        // program counter
    uint16_t stack[16]; // stack
    uint8_t sp;         // stack pointer
    bool gfx[64 * 32];  // display
    bool draw_flag;     // flag when the display needs redraw
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
    printf("Chip8 initialized\n");

    srand(time(NULL));
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

    printf("ROM loaded\n");
    return 0;
}

void cleanup(chip8 *chip) {
    // Clean up
    free(chip->memory);
    free(chip);
}

static void display_clear(chip8 *chip) {
    // Clears the screen
    memset(chip->gfx, 0, sizeof(chip->gfx));
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
        // TODO: Implement opcodes (24/35)

    case 0x0000:
        switch (chip->opcode & 0x000F) {
        case 0x0000: // 00E0: Clear the display
            display_clear(chip);
            chip->PC += 2;
            printf("Opcode (0x%X) called\n", chip->opcode);
            break;

        case 0x000E: // 00EE: Returns from a subroutine
            /* Return from a subroutine.
            The interpreter sets the program counter to the address at the top
            of the stack, then subtracts 1 from the stack pointer.
             */
            chip->PC = chip->sp--;
            printf("Opcode (0x%X) called\n", chip->opcode);
            break;

        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
            chip->PC += 2;
        }
        break;
    case 0x1000: // 1NNN: Jumps to address NNN
        /* Jump to location nnn.
        The interpreter sets the program counter to nnn.
         */
        chip->PC = chip->opcode & 0x0FFF;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;

    case 0x2000: // 2NNNN: Calls subroutine at NNN
        /* Call subroutine at nnn.
        The interpreter increments the stack pointer, then puts the current PC
        on the top of the stack. The PC is then set to nnn.*/
        chip->stack[++chip->sp] = chip->PC;
        chip->PC = chip->opcode & 0x0FFF;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;

    case 0x3000: { // 3XKK
        /* Skip next instruction if Vx = kk.

        The interpreter compares register Vx to kk, and if they are equal,
        increments the program counter by 2.
         */
        uint8_t x = (chip->opcode & 0x0F00) >> 8;
        if (chip->V[x] == (chip->opcode & 0x00FF))
            chip->PC += 2;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;
    }

    case 0x4000: { // 4XNN: Skips the next instruction if VX does not equals NN
        /* Skip next instruction if Vx != kk.

        The interpreter compares register Vx to kk, and if they are not equal,
        increments the program counter by 2.
         */
        uint8_t x = (chip->opcode & 0x0F00) >> 8;
        if (chip->V[x] != (chip->opcode & 0x00FF))
            chip->PC += 2;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;
    }

    case 0x5000: { // 5XY0
        /* Skip next instruction if Vx = Vy.

        The interpreter compares register Vx to register Vy, and if they are
        equal, increments the program counter by 2
         */
        uint8_t x = (chip->opcode & 0x0F00) >> 8;
        uint8_t y = chip->V[(chip->opcode & 0x00F0) >> 4];
        if (chip->V[x] == chip->V[y])
            chip->PC += 2;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;
    }

    case 0x6000: { // 6XKK
        /* Set Vx = kk.

        The interpreter puts the value kk into register Vx.
        */
        uint8_t x = (chip->opcode & 0x0F00) >> 8;
        chip->V[x] = chip->opcode & 0x00FF;
        chip->PC += 2;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;
    }

    case 0x7000: { // 7XKK
        /* Set Vx = Vx + kk.

        Adds the value kk to the value of register Vx, then stores the result in
        Vx.
         */
        uint8_t Vx = (chip->opcode & 0x0F00) >> 8;
        chip->V[Vx] += chip->opcode & 0x00FF;
        chip->PC += 2;
        printf("Opcode (0x%X) called \n", chip->opcode);
        break;
    }

    case 0x8000:
        switch (chip->opcode & 0x000F) {
        case 0x0000: { // 8XY0
            /* Set Vx = Vy.

            Stores the value of register Vy in register Vx
             */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            uint8_t y = (chip->opcode & 0x00F0) >> 4;
            chip->V[x] = chip->V[y];
            chip->PC += 2;
            printf("Opcode (0x%X) called\n", chip->opcode);
            break;
        }
        case 0x0001: { // 8xy1
            /* Set Vx = Vx OR Vy.

            Performs a bitwise OR on the values of Vx and Vy, then stores the
            result in Vx.
            */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            uint8_t y = (chip->opcode & 0x00F0) >> 4;
            chip->V[x] |= chip->V[y];
            chip->PC += 2;
            printf("Opcode (0x%X) called\n", chip->opcode);
            break;
        }
        case 0x0002: { // 8xy2
            /* Set Vx = Vx AND Vy.

            Performs a bitwise AND on the values of Vx and Vy, then stores the
            result in Vx.
            */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            uint8_t y = (chip->opcode & 0x00F0) >> 4;
            chip->V[x] &= chip->V[y];
            chip->PC += 2;
            printf("Opcode (0x%X) called\n", chip->opcode);
            break;
        }
        case 0x0003: { // 8xy3
            /*Set Vx = Vx XOR Vy.

            Performs a bitwise exclusive OR on the values of Vx and Vy, then
            stores the result in Vx
            */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            uint8_t y = (chip->opcode & 0x00F0) >> 4;
            chip->V[x] ^= chip->V[y];
            chip->PC += 2;
            printf("Opcode (0x%X) called\n", chip->opcode);
            break;
        }
        case 0x0004: { // 8xy4
            /* Set Vx = Vx + Vy, set VF = carry.

            The values of Vx and Vy are added together. If the result is greater
            than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
             */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            uint8_t y = (chip->opcode & 0x00F0) >> 4;
            uint16_t sum = chip->V[x] + chip->V[y];
            chip->V[0xF] = (sum > 255) ? 1 : 0;
            chip->V[x] = sum & 0xFF; // store the lowest 8 bit of the sum
            chip->PC += 2;
            printf("Opcode (0x%X) called\n", chip->opcode);
            break;
        }
        case 0x0005: { // 8xy5
            /*Set Vx = Vx - Vy, set VF = NOT borrow.

            If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted
            from Vx, and the results stored in Vx.
             */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            uint8_t y = (chip->opcode & 0x00F0) >> 4;
            chip->V[0xF] = 0;

            chip->V[0xF] = (chip->V[x] > chip->V[y]) ? 1 : 0;
            chip->V[x] -= chip->V[y];
            printf("Opcode (0x%X) called\n", chip->opcode);
            chip->PC += 2;
            break;
        }
        case 0x0006: { // 8xy6
            /* Set Vx = Vx SHR 1.

            If the least-significant bit of Vx is 1, then VF is set to 1,
            otherwise 0. Then is Vx divided by 2.
            */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            chip->V[0xF] = chip->V[x] & 0x1; // LSB
            chip->V[x] >>= 1;
            chip->PC += 2;
            printf("Opcode (0x%X) called\n", chip->opcode);
            break;
        }
        case 0x0007: { // 8xy7
            /* Set Vx = Vy - Vx, set VF = NOT borrow.

            If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted
            from Vy, and the results stored in Vx.
             */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            uint8_t y = (chip->opcode & 0x00F0) >> 4;
            chip->V[x] = chip->V[y] - chip->V[x];
            chip->V[0xF] = (chip->V[y] > chip->V[x]) ? 1 : 0;
            printf("Opcode (0x%X) called\n", chip->opcode);
            chip->PC += 2;
            break;
        }
        case 0x000E: { // 8xyE
            /* Set Vx = Vx SHL 1.

            If the most-significant bit of Vx is 1, then VF is set to 1,
            otherwise to 0. Then Vx is multiplied by 2.
            */
            uint8_t x = (chip->opcode & 0x0F00) >> 8;
            chip->V[0xF] = (chip->V[x] & 0x80); // MSB
            chip->V[x] <<= 1;
            printf("Opcode (0x%X) called\n", chip->opcode);
            chip->PC += 2;
            break;
        }
        default:
            printf("Unknown opcode [0x8000]: 0x%X\n", chip->opcode);
            chip->PC += 2;
        }
        break;

    case 0x9000: { // 9XY0: Skips the next instruction if VX does not equal VY
        uint8_t x = (chip->opcode & 0x0F00) >> 8;
        uint8_t y = (chip->opcode & 0x00F0) >> 4;
        if (chip->V[x] != chip->V[y])
            chip->PC += 2;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;
    }
    case 0xA000: // ANNN: Sets I to the address NNN
        chip->I = chip->opcode & 0x0FFF;
        chip->PC += 2; // Skip 2 since we read 2 memory address
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;

    case 0xB000: // BNNN: Jumps to the address NNN plus V0
        chip->PC = chip->opcode & 0x0FFF + chip->V[0x0];
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;

    case 0xC000: { // Cxkk:
        /* Set Vx = random byte AND kk.

        The interpreter generates a random number from 0 to 255, which is then
        ANDed with the value kk. The results are stored in Vx.
        */
        uint8_t rnd = (uint8_t)rand() % 255;

        uint8_t x = (chip->opcode & 0x0F00) >> 8;
        uint8_t kk = (chip->opcode & 0x00FF);
        chip->V[x] = rnd & kk;
        chip->PC += 2;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;
    }
    case 0xD000: { // Dxyn:
        /* Sprites are 8 pixels wide and have between 1-15 pixels height (N)
         * Sprites start at I location
         * If there is a collision, VF is set to 1
         * Draw to V[x] and V[y]
         */

        uint8_t x = (chip->opcode & 0x0F00) >> 8;
        uint8_t y = (chip->opcode & 0x00F0) >> 4;
        uint8_t height = (chip->opcode & 0x000F); // n
        uint8_t sprite_byte; // 1 byte, since each sprite is 8 pixels wide
        chip->V[0xF] = 0;

        // Iterate through each row of the sprite
        for (int row = 0; row < height; row++) {
            sprite_byte = chip->memory[chip->I + row]; // All width bits
            for (int col = 0; col < 8; col++) {
                if (sprite_byte >> (7 - col) & 1) { // Check if bit is 1
                    int index = ((chip->V[y] + row) % 32) * 64 +
                                ((chip->V[x] + col) % 64);
                    // Check if the bit on the display is already set
                    if (chip->gfx[index]) {
                        chip->V[0xF] = 1;
                    }
                    chip->gfx[index] ^= 1; // We know the bit is 1
                }
            }
        }
        chip->draw_flag = 1;
        chip->PC += 2;
        printf("Opcode (0x%X) called\n", chip->opcode);
        break;
    }
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

void drawGrapics(chip8 *chip) {
    if (chip->draw_flag) {
        // update screen
        chip->draw_flag = 0;
    }
}
