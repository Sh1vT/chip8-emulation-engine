# Chip-8 Emulator

This is a Chip-8 emulator written in C++.
The implementation is based on concepts from Austin Morlan’s [Chip-8 emulator project](https://code.austinmorlan.com/austin/2019-chip8-emulator).

## Requirements

* CMake
* A C++ compiler (GCC, Clang, or MSVC)
* SDL2 development libraries (`libsdl2-dev` on Linux or the equivalent for your system)

## Building

```bash
cmake -B build
cmake --build build
```

The executable will be generated in `build` directory.

## Running

```bash
./chip8 <path_to_rom>
```

## Notes

* Tested using the Chip-8 test ROMs from [corax89](https://github.com/corax89/chip8-test-rom).
* Sound output may not work on some systems depending on terminal or audio support. I might implement audio with SDL2 later.

