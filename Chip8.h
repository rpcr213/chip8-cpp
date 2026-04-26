#pragma once
#include "CPU.h"
#define KEYBOARD_SIZE 16

class Chip8 {
    private:
        CPU cpu;
        uint8_t keys[KEYBOARD_SIZE];
    public:
        Chip8();
        ~Chip8();
        int run_chip8();
        int get_keys();
};