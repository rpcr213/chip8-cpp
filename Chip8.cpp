#include "Chip8.h"
#include <cstring>
#include <cmath>
#include <SDL2/SDL.h>

static int chip8_key_from_sdl(SDL_Keycode key) {
    switch (key) {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;
        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xD;
        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xE;
        case SDLK_z: return 0xA;
        case SDLK_x: return 0x0;
        case SDLK_c: return 0xB;
        case SDLK_v: return 0xF;
        default: return -1;
    }
}

Chip8::Chip8() : window(nullptr), renderer(nullptr), texture(nullptr), audio_device(0), audio_enabled(false), audio_phase(0.0), sdl_ready(false), running(false) {
    for (int i = 0; i < KEYBOARD_SIZE; i++) {
        keys[i] = 0;
    }
    for (int i = 0; i < FB_X * FB_Y; i++) {
        fb[i] = 0;
        pixels[i] = 0xFF000000;
    }
}

int Chip8::run_chip8() {
    if (!sdl_ready && init_sdl() != 0) {
        return 1;
    }

    using clock = std::chrono::steady_clock;

    constexpr int CPU_HZ = CPU_PRED_HZ;
    constexpr int TIMER_HZ = TIMER_PRED_HZ;
    constexpr int INSTRUCTIONS_PER_TIMER_TICK = CPU_HZ / TIMER_HZ;

    const auto timer_interval = std::chrono::duration<double>(1.0 / TIMER_HZ);

    running = true;

    while (running) {
        auto frame_start = clock::now();

        get_keys();

        for (int i = 0; i < INSTRUCTIONS_PER_TIMER_TICK; i++) {
            if (cpu.process(keys) != 0) {
                running = false;
                break;
            }
        }

        cpu.tick_timers();
        update_audio();
        render_frame();

        auto frame_end = clock::now();
        auto elapsed = frame_end - frame_start;

        if (elapsed < timer_interval) {
            std::this_thread::sleep_for(timer_interval - elapsed);
        }
    }

    return 0;
}

int Chip8::init_sdl() {
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        return 1;
    }

    window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        FB_X * WINDOW_SCALE,
        FB_Y * WINDOW_SCALE,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (window == nullptr) {
        shutdown_sdl();
        return 1;
    }

    renderer = SDL_CreateRenderer(static_cast<SDL_Window*>(window), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer(static_cast<SDL_Window*>(window), -1, SDL_RENDERER_SOFTWARE);
    }
    if (renderer == nullptr) {
        shutdown_sdl();
        return 1;
    }

    SDL_RenderSetLogicalSize(static_cast<SDL_Renderer*>(renderer), FB_X, FB_Y);

    texture = SDL_CreateTexture(
        static_cast<SDL_Renderer*>(renderer),
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        FB_X,
        FB_Y
    );
    if (texture == nullptr) {
        shutdown_sdl();
        return 1;
    }

    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 512;
    want.callback = Chip8::audio_callback;
    want.userdata = this;

    audio_device = SDL_OpenAudioDevice(nullptr, 0, &want, nullptr, 0);
    if (audio_device != 0) {
        SDL_PauseAudioDevice(audio_device, 0);
    }

    sdl_ready = true;
    return 0;
}

void Chip8::shutdown_sdl() {
    if (audio_device != 0) {
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
    }
    if (texture != nullptr) {
        SDL_DestroyTexture(static_cast<SDL_Texture*>(texture));
        texture = nullptr;
    }
    if (renderer != nullptr) {
        SDL_DestroyRenderer(static_cast<SDL_Renderer*>(renderer));
        renderer = nullptr;
    }
    if (window != nullptr) {
        SDL_DestroyWindow(static_cast<SDL_Window*>(window));
        window = nullptr;
    }
    if (sdl_ready) {
        SDL_Quit();
        sdl_ready = false;
    }
}

void Chip8::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            continue;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
            continue;
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            int chip8_key = chip8_key_from_sdl(event.key.keysym.sym);
            if (chip8_key >= 0) {
                keys[chip8_key] = (event.type == SDL_KEYDOWN) ? 1 : 0;
            }
        }
    }
}

void Chip8::render_frame() {
    if (renderer == nullptr || texture == nullptr) {
        return;
    }

    cpu.copy_fb(fb);
    for (int i = 0; i < FB_X * FB_Y; i++) {
        pixels[i] = fb[i] ? 0xFFFFFFFF : 0xFF000000;
    }

    SDL_UpdateTexture(static_cast<SDL_Texture*>(texture), nullptr, pixels, FB_X * sizeof(uint32_t));
    SDL_SetRenderDrawColor(static_cast<SDL_Renderer*>(renderer), 0, 0, 0, 255);
    SDL_RenderClear(static_cast<SDL_Renderer*>(renderer));
    SDL_RenderCopy(static_cast<SDL_Renderer*>(renderer), static_cast<SDL_Texture*>(texture), nullptr, nullptr);
    SDL_RenderPresent(static_cast<SDL_Renderer*>(renderer));
}

void Chip8::update_audio() {
    audio_enabled.store(cpu.sound_timer_active(), std::memory_order_relaxed);
}

void Chip8::audio_callback(void* userdata, uint8_t* stream, int len) {
    Chip8* chip8 = static_cast<Chip8*>(userdata);
    int16_t* samples = reinterpret_cast<int16_t*>(stream);
    int sample_count = len / static_cast<int>(sizeof(int16_t));
    constexpr double sample_rate = 44100.0;
    constexpr double frequency = 440.0;
    constexpr int16_t volume = 3000;

    if (!chip8->audio_enabled.load(std::memory_order_relaxed)) {
        std::memset(stream, 0, len);
        return;
    }

    for (int i = 0; i < sample_count; i++) {
        samples[i] = chip8->audio_phase < 0.5 ? volume : -volume;
        chip8->audio_phase += frequency / sample_rate;
        if (chip8->audio_phase >= 1.0) {
            chip8->audio_phase -= std::floor(chip8->audio_phase);
        }
    }
}

void Chip8::get_keys() {
    handle_events();
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
    shutdown_sdl();
}
