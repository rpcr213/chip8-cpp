#include <iostream>
#include "Chip8.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "./chip8 <rom_name.ch8>" << endl;
        return 1;
    }
    Chip8 chip8 = Chip8();

    tErrRom rom_err = chip8.load_rom(argv[1]);
    switch (rom_err) {
        case ERR_NAME:
            cout << "Error: Rom: " << argv[1] << " not found" << endl;
            return 1;
        case ERR_READ:
            cout << "Error: Unable to read" << endl;
            return 1;
        case ERR_SIZE:
            cout << "Error: Rom too big" << endl;
            return 1;
        case ERR_SIZE_CPU:
            cout << "Error: Rom too big (cpu)" << endl;
            return 1;
        case ERR_ROM_PTR:
            cout << "Error: invalid rom pointer" << endl;
            return 1;
        default:
            cout << "ROM OK" << endl;
            break;
    }

    int err = chip8.run_chip8();
    cout << "END CHIP8" << endl;

    return err;
}
