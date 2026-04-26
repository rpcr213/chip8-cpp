#include "Chip8.h"

Chip8::Chip8() {
    CPU();
    for (int i = 0; i < KEYBOARD_SIZE; i++) {
        keys[i] = 0;
    }
}

int Chip8::run_chip8() {

}

int Chip8::get_keys() {

}