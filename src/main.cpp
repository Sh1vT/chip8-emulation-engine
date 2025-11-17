#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "Chip8.hpp"
#include "SDL2Display.hpp"

int main(int argc, char** argv){

    if(argc!=2){
        std::cout << "Usage: " << argv[0] << " <ROM file>\n";
        return 1;
    }

    std::string romPath = argv[1];

    Chip8 chip8;
    chip8.loadROM(romPath);

    SDL2Display display(10);

    bool quit = false;

    using clock = std::chrono::high_resolution_clock;
    auto lastTimerUpdate = clock::now();

    //CPU ~500 Hz
    const int CPU_HZ = 500;
    const auto CPU_DELAY = std::chrono::microseconds(1000000/CPU_HZ);

    // Timers 60 Hz
    const int TIMER_HZ = 60;
    const auto TIMER_DELAY = std::chrono::microseconds(1000000 / TIMER_HZ);

    while(!quit){

        auto cycleStart = clock::now();

        //input
        display.processInput(chip8.keypad, quit);
        if(quit) break;

        //1 cpu cycle
        chip8.cycle();

        //60 hz timers
        auto now = clock::now();
        if(now-lastTimerUpdate >= TIMER_DELAY){
            if (chip8.delayTimer > 0) chip8.delayTimer--;
            if (chip8.soundTimer > 0) {
                chip8.soundTimer--;
                std::cout << "\a" << std::flush; //terminal beep
            }
            lastTimerUpdate = now;
        }

        //draw frame
        display.draw(chip8.video);

        auto cycleEnd = clock::now();
        auto elapsed = cycleEnd-cycleStart;
        if(elapsed<CPU_DELAY) {
            std::this_thread::sleep_for(CPU_DELAY-elapsed);
        }

    }

    return 0;
}