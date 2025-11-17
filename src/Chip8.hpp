#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <string>

/*

    Chip8 has:
    16 8bit registers
    4K bytes of memory
    16 bit index register
    16 bit program counter
    16 level stack 
    8 bit stack pointer
    8 bit delay timer
    8 bit sound timer
    16 input keys
    64x32 monochrome display
    16 bit running opcode

*/

class Chip8 {
public:
    uint8_t registers[16];
    uint8_t memory[4096];

    uint16_t index;
    uint16_t PC;
    
    uint16_t stack[16];
    uint8_t sp{};
    
    uint8_t delayTimer;
    uint8_t soundTimer;
    
    uint8_t keypad[16];
    uint32_t video[64*32];
    uint16_t opcode;

    Chip8();
    void loadROM(const std::string &filename);
    void cycle();
};

#endif