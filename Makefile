build:
	gcc -std=c23 chip8.c main.c -o chip8emu -lSDL3
run:
	./chip8emu
