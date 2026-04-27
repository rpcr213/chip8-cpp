#include "Chip8.h"

Chip8::Chip8() {
    CPU();
    for (int i = 0; i < KEYBOARD_SIZE; i++) {
        keys[i] = 0;
    }
}

int Chip8::run_chip8() {
    using clock = std::chrono::steady_clock;

    constexpr int CPU_HZ = CPU_PRED_HZ;
    constexpr int TIMER_HZ = TIMER_PRED_HZ;
    constexpr int INSTRUCTIONS_PER_TIMER_TICK = CPU_HZ / TIMER_HZ;

    const auto timer_interval = std::chrono::duration<double>(1.0 / TIMER_HZ);

    bool running = true;

    while (running) {
        auto frame_start = clock::now();

        get_keys();

        for (int i = 0; i < INSTRUCTIONS_PER_TIMER_TICK; i++) {
            cpu.process(keys);
        }

        cpu.tick_timers();

        auto frame_end = clock::now();
        auto elapsed = frame_end - frame_start;

        if (elapsed < timer_interval) {
            std::this_thread::sleep_for(timer_interval - elapsed);
        }
    }

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
