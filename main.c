#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Usage: chip8emu <gamefile>");
        return EXIT_SUCCESS;
    }

    chip8 *chip = initialize();

    load_game(chip, argv[1]);

    // Load Game File from argument

    // Emulation loop
    for (;;) {
        // 1. Emulate Cycle
        emulateCycle(chip);
        sleep(1);
        // 2. Update screen, if draw flag is set
        // 3. Store key press state
    }
    cleanup(chip);
    return 0;
}
