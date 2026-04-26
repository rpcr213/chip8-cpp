#include "Chip8.h"

Chip8::Chip8() {
    CPU();
    for (int i = 0; i < KEYBOARD_SIZE; i++) {
        keys[i] = 0;
    }
}

int Chip8::run_chip8() {
    
    return 0;
}

int Chip8::get_keys() {

    return 0;
}

tErrRom Chip8::load_rom(char* path) {
    int rom_fd = open(path, O_RDONLY);

    if (rom_fd < 0) {
        return ERR_NAME;
    }

    uint8_t u;
    int cont = 0;
    ssize_t n;

    while ((n = read(rom_fd, &u, 1)) > 0) {
        if (cont >= MEMORY_SIZE - ROM_START) {
            close(rom_fd);
            return ERR_SIZE;
        }

        rom[cont] = u;
        cont++;
    }

    close(rom_fd);

    if (n < 0) {
        return ERR_READ;
    }

    return cpu.load_rom(rom, cont);
}


Chip8::~Chip8() {

}
