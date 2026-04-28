#pragma once
#include "CPU.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <chrono>
#include <thread>
#include <atomic>
#define KEYBOARD_SIZE 16
#define CPU_PRED_HZ 600
#define TIMER_PRED_HZ 60
#define WINDOW_SCALE 10



class Chip8 {
    private:
        CPU cpu;
        uint8_t keys[KEYBOARD_SIZE];
        uint8_t rom[MEMORY_SIZE];
        uint8_t fb[FB_X * FB_Y];
        uint32_t pixels[FB_X * FB_Y];
        void* window;
        void* renderer;
        void* texture;
        uint32_t audio_device;
        std::atomic<bool> audio_enabled;
        double audio_phase;
        bool sdl_ready;
        bool running;

        int init_sdl();
        void shutdown_sdl();
        void handle_events();
        void render_frame();
        void update_audio();
        static void audio_callback(void* userdata, uint8_t* stream, int len);
    public:
        Chip8();
        ~Chip8();
        tErrRom load_rom(char* path);
        int run_chip8();
        void get_keys();
};
