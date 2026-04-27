#pragma once
#include "CPU.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <chrono>
#include <thread>
#define KEYBOARD_SIZE 16
#define CPU_PRED_HZ 600
#define TIMER_PRED_HZ 60



class Chip8 {
    private:
        CPU cpu;
        uint8_t keys[KEYBOARD_SIZE];
        uint8_t rom[MEMORY_SIZE];
    public:
        Chip8();
        ~Chip8();
        tErrRom load_rom(char* path);
        int run_chip8();
        int get_keys();
};