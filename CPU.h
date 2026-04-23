#pragma once
#include <stdint.h>
#include "sprites.h"
#define REG_COUNT 16
#define STACK_SIZE 16
#define FB_X 64
#define FB_Y 32
#define MEMORY_SIZE 4096


class CPU {
    private:
        uint8_t v[REG_COUNT]; // VX register: V0, V1, V2, ..., VF
        uint16_t ir; // index register
        uint16_t pc; // program counter
        uint8_t sp; // stack pointer
        uint8_t dt; // delay timer
        uint8_t st; // sound timer
        uint8_t fb[FB_X * FB_Y]; // frame buffer
        uint16_t stack[STACK_SIZE]; // 16 bits porque las direcciones ocupan 12, uint8_t -> insuficiente
        uint8_t memory[MEMORY_SIZE];
        int execute_instruction(uint16_t opcode);


    public:
        CPU();
        ~CPU();
        int process();


};