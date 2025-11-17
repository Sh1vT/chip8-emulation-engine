#include <Chip8.hpp>
#include <fstream>
#include <iostream>
#include <random>

/*

This is the cpu file of chip8. Its supposed to implement the header of same name, plus define some free functions for managing 
chip8 opcodes. Theres exactly 34 opcodes, and the notation of opcode is present in docs for convenience.

OPCODES

Any chip8 opcode is of 4 hexplaces / 16 bits
first hexplace / 4 bits is the type of instruction, next 3 hexplace / 12 bits are the address

So every instruction has this structure:
OP A B C
↑  └─── 12-bit argument (addr, value, register pair, etc.)
└────── 4-bit operation code

most opcode functions will require the opcode to perform calc on address, some specific ones like CLS and RET dont need it

*/


//00E0: CLS
void OP_00E0 (Chip8 &c) {
    std::fill(std::begin(c.video), std::end(c.video), 0);
}

//00EE: RET
void OP_00EE (Chip8 &c) {
    c.sp--;
    c.PC = c.stack[c.sp];
}

//1nnn: JP addr
void OP_1nnn(Chip8 &c, uint16_t opcode) {
    c.PC = opcode & 0x0FFF; //RHS extracts the 3 right hexplaces
}

//2nnn - CALL addr
void OP_2nnn(Chip8 &c, uint16_t opcode){
    c.stack[c.sp] = c.PC;
    c.sp++; 
    c.PC = opcode & 0x0FFF;
}

//3xkk - SE Vx, byte
void OP_3xkk(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8; //register index
    uint8_t byte = opcode & 0x00FF;
    if (c.registers[x]==byte) c.PC+=2;
}

//4xkk - SNE Vx, byte
void OP_4xkk(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    if (c.registers[x]!=byte) c.PC+=2;
}

//5xy0 - SE Vx, Vy
void OP_5xy0(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    if (c.registers[x]==c.registers[y]) c.PC+=2;
}

//6xkk - LD Vx, byte
void OP_6xkk(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    c.registers[x] = byte;
}

//7xkk - ADD Vx, byte
void OP_7xkk(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    c.registers[x] += byte;
}

//8xy0 - LD Vx, Vy
void OP_8xy0(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    c.registers[x] = c.registers[y];
}

//8xy1 - OR Vx, Vy
void OP_8xy1(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    c.registers[x] |= c.registers[y];
    c.registers[0xF] = 0;
}

//8xy2 - AND Vx, Vy
void OP_8xy2(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    c.registers[x] &= c.registers[y];
    c.registers[0xF] = 0;
}

//8xy3 - XOR Vx, Vy
void OP_8xy3(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    c.registers[x] ^= c.registers[y];
    c.registers[0xF] = 0;
}

//8xy4 - ADD Vx, Vy
void OP_8xy4(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint16_t sum = c.registers[x] + c.registers[y];
    if(sum>255) c.registers[0xF] = 1;
    else c.registers[0xF] = 0;
    c.registers[x] = sum & 0xFF; //update to the lower 8 bits of sum
}

//8xy5 - SUB Vx, Vy
void OP_8xy5(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    if(c.registers[x]>c.registers[y]) c.registers[0xF] = 1;
    else c.registers[0xF] = 0;
    c.registers[x] -= c.registers[y];
}

//8xy6 - SHR Vx
void OP_8xy6(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    if (c.registers[x] & 0x1) c.registers[0xF] = 1;
    else c.registers[0xF] = 0;
    c.registers[x] >>= 1;
}

//8xy7 - SUBN Vx, Vy
void OP_8xy7(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    if(c.registers[y]>c.registers[x]) c.registers[0xF] = 1;
    else c.registers[0xF] = 0;
    c.registers[x] = c.registers[y] - c.registers[x];
}

//8xyE - SHL Vx {, Vy}
void OP_8xyE(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    if (c.registers[x] & 0x80) c.registers[0xF] = 1;
    else c.registers[0xF] = 0;
    c.registers[x] <<= 1;
}

//9xy0 - SNE Vx, Vy
void OP_9xy0(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    if(c.registers[x]!=c.registers[y]) c.PC += 2;
}

//Annn - LD I, addr
void OP_Annn(Chip8 &c, uint16_t opcode) {
    uint16_t address = opcode & 0x0FFF;
    c.index = address;
}

//Bnnn - JP V0, addr
void OP_Bnnn(Chip8 &c, uint16_t opcode) {
    uint16_t address = opcode & 0x0FFF;
    c.PC = c.registers[0] + address;
}

//Cxkk - RND Vx, byte
void OP_Cxkk(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;
    uint8_t randByte = rand() % 256; //generate random byte 0–255
    c.registers[x] = randByte & kk;
}

//Dxyn - DRW Vx, Vy, nibble
void OP_Dxyn(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t height = opcode & 0x000F;

    uint8_t xpos = c.registers[x] % 64;
    uint8_t ypos = c.registers[y] % 32;

    c.registers[0xF] = 0;
    
    for(int row = 0; row<height; row++){
        uint8_t spriteByte = c.memory[c.index+row];

        for (int col=0; col<8; col++) {
            uint8_t spritePixel = spriteByte >> (7-col) & 1;

            if (spritePixel==1) {
                int x = (xpos + col) % 64;
                int y = (ypos + row) % 32;
                int pixelIndex = y*64 + x;

                if (c.video[pixelIndex]==1) c.registers[0xF] = 1;

                c.video[pixelIndex] ^=1;
            }
            
        }
        
    }
}

//Ex9E - SKP Vx
void OP_Ex9E(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t key = c.registers[x];
    if (c.keypad[key]==1) c.PC+=2;
}

//ExA1 - SKNP Vx
void OP_ExA1(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t key = c.registers[x];
    if (c.keypad[key]!=1) c.PC+=2;
}

//Fx07 - LD Vx, DT
void OP_Fx07(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    c.registers[x] = c.delayTimer;
}

//Fx0A - LD Vx, K - easiest way to wait is to decrement PC by 2 repeatedly whenever a keypad value is not detected
void OP_Fx0A(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int key = 0; key < 16; key++) {
        if (c.keypad[key]) {
            c.registers[x] = key;
            return;             // stop here; key was found
        }
    }

    c.PC-=2;
}

//Fx15 - LD DT, Vx
void OP_Fx15(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    c.delayTimer = c.registers[x];
}

//Fx18 - LD ST, Vx
void OP_Fx18(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    c.soundTimer = c.registers[x];
}

//Fx1E - ADD I, Vx
void OP_Fx1E(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    c.index += c.registers[x];
}

//Fx29 - LD F, Vx
void OP_Fx29(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t digit = c.registers[x];
    c.index = digit*5;
}

//Fx33 - LD B, Vx
void OP_Fx33(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t num = c.registers[x];

    //ones place
    c.memory[c.index+2] = num % 10;
    num /= 10;

    //tens place
    c.memory[c.index+1] = num % 10;
    num /= 10;

    //hundreds place
    c.memory[c.index+0] = num % 10;
}

//Fx55 - LD [I], Vx
void OP_Fx55(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for(int i=0; i<=x; i++){
        c.memory[c.index+i] = c.registers[i];
    }
}

//Fx65 - LD Vx, [I]
void OP_Fx65(Chip8 &c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for(int i=0; i<=x; i++){
        c.registers[i] = c.memory[c.index+i];
    }
}

Chip8::Chip8() {

    PC = 0x200; // instructions for any rom start from this address
    index = 0;  
    opcode = 0;
    sp = 0; //top of stack - initially 0

    delayTimer = 0; //these are decremented at 60hz in cycle
    soundTimer = 0;

    for(int i = 0; i<4096; i++) memory[i] = 0; //memory cleanup

    for(int i = 0; i<16; i++) registers[i] = 0; //register cleanup

    for(int i = 0; i<64*32; i++) video[i] = 0; //video-buffer cleanup

    for(int i = 0; i<16; i++) keypad[i] = 0; //keys unpressed

    for(int i = 0; i<16; i++) stack[i] = 0; //stack cleanup

    //adding fontset to memory
    uint8_t fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    for (int i = 0; i < 80; i++) memory[i] = fontset[i];

}

void Chip8::loadROM(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);

    if(!file){
        std::cout << "Cannot open ROM : " << filename << std::endl;
        return;
    }

    //file is present
    //starting address would be 0x200

    int pos = 0x200;
    char byte; // just a container to grab bytes of file - one by one, unrealted to character

    while (file.get(byte)) {
        if(pos>=4096) {
            std::cout<< "ROM too large" << std::endl;
            break;
        }
        // memory[pos] = byte; can work but might vary on systems and be inconsistent
        memory[pos] = static_cast<uint8_t>(byte); 
        pos++;
    }
}

void Chip8::cycle() {
    opcode = (memory[PC] << 8) | memory[PC+1];
    PC+=2;

    uint16_t category = opcode & 0xF000;

    switch (category)
    {
    case 0x0000:
        switch (opcode & 0x00FF) {
            case 0xE0: OP_00E0(*this); break;
            case 0xEE: OP_00EE(*this); break;
            default:
                std::cout << "Unknown 0x0 opcode: " << std::hex << opcode << "\n";
        }
        break;

    case 0x1000: OP_1nnn(*this, opcode); break;
    case 0x2000: OP_2nnn(*this, opcode); break;
    case 0x3000: OP_3xkk(*this, opcode); break;
    case 0x4000: OP_4xkk(*this, opcode); break;

    case 0x5000:
        if ((opcode & 0x000F) == 0)
            OP_5xy0(*this, opcode);
        break;

    case 0x6000: OP_6xkk(*this, opcode); break;
    case 0x7000: OP_7xkk(*this, opcode); break;

    case 0x8000:
        switch (opcode & 0x000F) {
            case 0x0: OP_8xy0(*this, opcode); break;
            case 0x1: OP_8xy1(*this, opcode); break;
            case 0x2: OP_8xy2(*this, opcode); break;
            case 0x3: OP_8xy3(*this, opcode); break;
            case 0x4: OP_8xy4(*this, opcode); break;
            case 0x5: OP_8xy5(*this, opcode); break;
            case 0x6: OP_8xy6(*this, opcode); break;
            case 0x7: OP_8xy7(*this, opcode); break;
            case 0xE: OP_8xyE(*this, opcode); break;
        }
        break;

    case 0x9000:
        if ((opcode & 0x000F) == 0)
            OP_9xy0(*this, opcode);
        break;

    case 0xA000: OP_Annn(*this, opcode); break;
    case 0xB000: OP_Bnnn(*this, opcode); break;
    case 0xC000: OP_Cxkk(*this, opcode); break;
    case 0xD000: OP_Dxyn(*this, opcode); break;

     case 0xE000:
        switch (opcode & 0x00FF) {
            case 0x9E: OP_Ex9E(*this, opcode); break;
            case 0xA1: OP_ExA1(*this, opcode); break;
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF) {
            case 0x07: OP_Fx07(*this, opcode); break;
            case 0x0A: OP_Fx0A(*this, opcode); break;
            case 0x15: OP_Fx15(*this, opcode); break;
            case 0x18: OP_Fx18(*this, opcode); break;
            case 0x1E: OP_Fx1E(*this, opcode); break;
            case 0x29: OP_Fx29(*this, opcode); break;
            case 0x33: OP_Fx33(*this, opcode); break;
            case 0x55: OP_Fx55(*this, opcode); break;
            case 0x65: OP_Fx65(*this, opcode); break;
        }
        break;

    default:
        std::cout << "Unknown opcode: " << std::hex << opcode << "\n";
    }

    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}