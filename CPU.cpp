#include "CPU.h"

CPU::CPU() : pc(0x200), ir(0), sp(0), dt(0), st(0) {
    // variamos memoria
    for (int i = 0; i < REG_COUNT; i++) {
        this->v[i] = 0;
    }
    for (int i = 0; i < FB_X * FB_Y; i++) {
        this->fb[i] = 0;
    }
    for (int i = 0; i < STACK_SIZE; i++) {
        this->stack[i] = 0;
    }
    for (int i = 0; i < MEMORY_SIZE; i++) {
        this->memory[i] = 0;
    }

    // aprovechamos los primeros espacios para almacenar los sprites
    for (int i = 0; i < SPRITES_ROWS; i++) {
        for (int j = 0; j < SPRITES_COLS ; j++) {
            this->memory[(i * SPRITES_COLS) + j] = sprites[i][j];
        }
    }
}

CPU::~CPU() {

}

int CPU::process() {
    // fetch
    if (pc >= MEMORY_SIZE) return 1;

    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    if (execute_instruction(opcode)) {
        return 1;
    }



    return 0;
}

int CPU::execute_instruction(uint16_t opcode) {

    uint8_t x  = (opcode & 0x0F00) >> 8;
    uint8_t y  = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t n  = opcode & 0x000F;

    switch (opcode & 0xF000) {

        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0xE0:
                    cls();
                    break;
                case 0xEE:
                    ret();
                    break;
                default:
                    sys_addr(nnn);
                    break;
            }
            break;

        case 0x1000:
            jp_addr(nnn);
            break;

        case 0x2000:
            call_addr(nnn);
            break;

        case 0x3000:
            se_vx_byte(x, kk);
            break;

        case 0x4000:
            sne_vx_byte(x, kk);
            break;

        case 0x5000:
            if ((opcode & 0x000F) == 0x0)
                se_vx_vy(x, y);
            break;

        case 0x6000:
            ld_vx_byte(x, kk);
            break;

        case 0x7000:
            add_vx_byte(x, kk);
            break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0:
                    ld_vx_vy(x, y);
                    break;
                case 0x1:
                    or_vx_vy(x, y);
                    break;
                case 0x2:
                    and_vx_vy(x, y);
                    break;
                case 0x3:
                    xor_vx_vy(x, y);
                    break;
                case 0x4:
                    add_vx_vy(x, y);
                    break;
                case 0x5:
                    sub_vx_vy(x, y);
                    break;
                case 0x6:
                    shr_vx(x);
                    break;
                case 0x7:
                    subn_vx_vy(x, y);
                    break;
                case 0xE:
                    shl_vx(x);
                    break;
            }
            break;

        case 0x9000:
            if ((opcode & 0x000F) == 0x0)
                sne_vx_vy(x, y);
            break;

        case 0xA000:
            ld_i_addr(nnn);
            break;

        case 0xB000:
            jp_v0_addr(nnn);
            break;

        case 0xC000:
            rnd_vx_byte(x, kk);
            break;

        case 0xD000:
            drw_vx_vy_nibble(x, y, n);
            break;

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x9E:
                    skp_vx(x);
                    break;
                case 0xA1:
                    sknp_vx(x);
                    break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07:
                    ld_vx_dt(x);
                    break;
                case 0x0A:
                    ld_vx_k(x);
                    break;
                case 0x15:
                    ld_dt_vx(x);
                    break;
                case 0x18:
                    ld_st_vx(x);
                    break;
                case 0x1E:
                    add_i_vx(x);
                    break;
                case 0x29:
                    ld_f_vx(x);
                    break;
                case 0x33:
                    ld_b_vx(x);
                    break;
                case 0x55:
                    ld_i_vx(x);
                    break;
                case 0x65:
                    ld_vx_i(x);
                    break;
            }
            break;

        default:
            // no deberia de entrar aqui
            break;
    }

    return 0;
}

void CPU::cls() {}
void CPU::ret() {}
void CPU::sys_addr(uint16_t addr) {}

void CPU::jp_addr(uint16_t addr) {}
void CPU::call_addr(uint16_t addr) {}

void CPU::se_vx_byte(uint8_t x, uint8_t kk) {}
void CPU::sne_vx_byte(uint8_t x, uint8_t kk) {}
void CPU::se_vx_vy(uint8_t x, uint8_t y) {}

void CPU::ld_vx_byte(uint8_t x, uint8_t kk) {}
void CPU::add_vx_byte(uint8_t x, uint8_t kk) {
    v[x] = v[x] + kk;
}

void CPU::ld_vx_vy(uint8_t x, uint8_t y) {
    v[x] = v[y];
}

void CPU::or_vx_vy(uint8_t x, uint8_t y) {
    v[x] = v[x] | v[y];
}

void CPU::and_vx_vy(uint8_t x, uint8_t y) {
    v[x] = v[x] & v[y];
}

void CPU::xor_vx_vy(uint8_t x, uint8_t y) {
    v[x] = v[x] ^ v[y];
}

// sumamos y si hay overflow, ponemos vf a 1
void CPU::add_vx_vy(uint8_t x, uint8_t y) {
    uint16_t res = v[x] + v[y]; 
    if (res > 0xFF) v[0xF] = 1;
    else v[0xF] = 0;
    v[x] = (res & 0xFF);
}

// restamos y si no hay overflow, ponemos vf a 1
void CPU::sub_vx_vy(uint8_t x, uint8_t y) {
    if (v[x] > v[y]) v[0xF] = 1;
    else v[0xF] = 0;
    v[x] = v[x] - v[y];
}

// v[x] /= 2 y si es impar (antes de dividir) vf pasa a ser 1
void CPU::shr_vx(uint8_t x) {
    v[0xF] = (v[x] & 0b1);
    v[x] = v[x] / 2;
}

// v[x] - v[y], si (v[y] > v[x]) entonces vf = 1
void CPU::subn_vx_vy(uint8_t x, uint8_t y) {
    v[0xF] = (v[y] > v[x]);
    v[x] = v[y] - v[x];
}

void CPU::shl_vx(uint8_t x) {
    v[0xF] = (v[x] & 0b10000000);
    v[x] = v[x] * 2;
}

void CPU::sne_vx_vy(uint8_t x, uint8_t y) {
    if (v[x] != v[y]) pc += 2;
}

void CPU::ld_i_addr(uint16_t addr) {}
void CPU::jp_v0_addr(uint16_t addr) {}

void CPU::rnd_vx_byte(uint8_t x, uint8_t kk) {}

void CPU::drw_vx_vy_nibble(uint8_t x, uint8_t y, uint8_t n) {}

void CPU::skp_vx(uint8_t x) {}
void CPU::sknp_vx(uint8_t x) {}

void CPU::ld_vx_dt(uint8_t x) {}
void CPU::ld_vx_k(uint8_t x) {}
void CPU::ld_dt_vx(uint8_t x) {}
void CPU::ld_st_vx(uint8_t x) {}
void CPU::add_i_vx(uint8_t x) {}
void CPU::ld_f_vx(uint8_t x) {}
void CPU::ld_b_vx(uint8_t x) {}
void CPU::ld_i_vx(uint8_t x) {}
void CPU::ld_vx_i(uint8_t x) {}