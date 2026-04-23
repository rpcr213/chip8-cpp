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
    // decode
    switch (opcode & 0xF000) { // TODO: 0NNN?
        case 0x0:
            switch (opcode & 0xFF) {
                case 0xE0:
                    // TODO
                    break;
                case 0xEE:
                    // TODO
                    break;
            }
            break;
        case 0x1:
            // TODO
            break;
        case 0x2:
            // TODO
            break;
        case 0x3:
            // TODO
            break;
        case 0x4:
            // TODO
            break;
        case 0x5:
            // TODO
            break;
        case 0x6:
            // TODO
            break;
        case 0x7:
            // TODO
            break;
        case 0x8:
            switch (opcode & 0xF) {
                case 0x0:
                    // TODO
                    break;
                case 0x1:
                    // TODO
                    break;
                case 0x2:
                    // TODO
                    break;
                case 0x3:
                    // TODO
                    break;
                case 0x4:
                    // TODO
                    break;
                case 0x5:
                    // TODO
                    break;
                case 0x6:
                    // TODO
                    break;
                case 0x7:
                    // TODO
                    break;
                case 0xE:
                    // TODO
                    break;
            }
            break;
        case 0x9:
            // TODO
            break;
        case 0xA:
            // TODO
            break;
        case 0xB:
            // TODO
            break;
        case 0xC:
            // TODO
            break;
        case 0xD:
            // TODO
            break;
        case 0xE:
            switch (opcode & 0x00FF) {
                case 0x9E:
                    // TODO
                    break;
                case 0x1A:
                    // TODO
                    break;
            }
            break;
        case 0xF:
            switch (opcode & 0x00FF) {
                case 0x07:
                    // TODO
                    break;
                case 0x0A:
                    // TODO
                    break;
                case 0x15:
                    // TODO
                    break;
                case 0x18:
                    // TODO
                    break;
                case 0x1E:
                    // TODO
                    break;
                case 0x29:
                    // TODO
                    break;
                case 0x33:
                    // TODO
                    break;
                case 0x55:
                    // TODO
                    break;
                case 0x65:
                    // TODO
                    break;
            }
            break;
        default:
            // TODO
            break;
    }

    return 0;
}