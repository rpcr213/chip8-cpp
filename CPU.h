#pragma once
#include <stdint.h>
#include <random>
#define SPRITES_ROWS 16
#define SPRITES_COLS 5
// #include "sprites.h"
#define REG_COUNT 16
#define STACK_SIZE 16
#define FB_X 64
#define FB_Y 32
#define MEMORY_SIZE 4096
#define SEED 12345
#define ROM_START 0x200

typedef enum {
    ROM_OK = 0,
    ERR_NAME = 1,
    ERR_SIZE = 2,
    ERR_READ = 3,
    ERR_SIZE_CPU = 4,
    ERR_ROM_PTR = 5,
} tErrRom;


class CPU {
    private:
        std::mt19937 gen;
        uint8_t v[REG_COUNT]; // VX register: V0, V1, V2, ..., VF
        uint16_t ir; // index register
        uint16_t pc; // program counter
        uint8_t sp; // stack pointer
        uint8_t dt; // delay timer
        uint8_t st; // sound timer
        uint8_t fb[FB_X * FB_Y]; // frame buffer
        uint16_t stack[STACK_SIZE]; // 16 bits porque las direcciones ocupan 12, uint8_t -> insuficiente
        uint8_t memory[MEMORY_SIZE];
        int execute_instruction(uint16_t opcode, const uint8_t* keys);
        
        // 0x0
        void cls();
        void ret();
        void sys_addr(uint16_t addr);

        // 0x1
        void jp_addr(uint16_t addr);

        // 0x2
        void call_addr(uint16_t addr);

        // 0x3
        void se_vx_byte(uint8_t x, uint8_t kk);

        // 0x4
        void sne_vx_byte(uint8_t x, uint8_t kk);

        // 0x5
        void se_vx_vy(uint8_t x, uint8_t y);

        // 0x6
        void ld_vx_byte(uint8_t x, uint8_t kk);

        // 0x7
        void add_vx_byte(uint8_t x, uint8_t kk);

        // 0x8
        void ld_vx_vy(uint8_t x, uint8_t y);
        void or_vx_vy(uint8_t x, uint8_t y);
        void and_vx_vy(uint8_t x, uint8_t y);
        void xor_vx_vy(uint8_t x, uint8_t y);
        void add_vx_vy(uint8_t x, uint8_t y);
        void sub_vx_vy(uint8_t x, uint8_t y);
        void shr_vx(uint8_t x);
        void subn_vx_vy(uint8_t x, uint8_t y);
        void shl_vx(uint8_t x);

        // 0x9
        void sne_vx_vy(uint8_t x, uint8_t y);

        // 0xA
        void ld_i_addr(uint16_t addr);

        // 0xB
        void jp_v0_addr(uint16_t addr);

        // 0xC
        void rnd_vx_byte(uint8_t x, uint8_t kk);

        // 0xD
        void drw_vx_vy_nibble(uint8_t x, uint8_t y, uint8_t n);

        // 0xE
        void skp_vx(uint8_t x, const uint8_t* keys);
        void sknp_vx(uint8_t x, const uint8_t* keys);

        // 0xF
        void ld_vx_dt(uint8_t x);
        void ld_vx_k(uint8_t x, const uint8_t* keys);
        void ld_dt_vx(uint8_t x);
        void ld_st_vx(uint8_t x);
        void add_i_vx(uint8_t x);
        void ld_f_vx(uint8_t x);
        void ld_b_vx(uint8_t x);
        void ld_i_vx(uint8_t x);
        void ld_vx_i(uint8_t x);


    public:
        CPU();
        ~CPU();
        int process(const uint8_t* keys);
        void copy_fb(uint8_t* fb);
        tErrRom load_rom(uint8_t* rom, uint16_t size);
        void tick_timers(); // se llama 60 veces en 1 seg

};