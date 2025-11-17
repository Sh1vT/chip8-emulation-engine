#ifndef SDL2DISPLAY_H
#define SDL2DISPLAY_H

#include <SDL2/SDL.h>
#include <cstdint>

/*

This is the display header file responsible for SDL2 display.
Its objective is to declare the UI of emulator

*/

class SDL2Display {
public:
    SDL2Display(int windowScale=10);
    ~SDL2Display();

    void draw(const uint32_t video[64*32]); //no reference required, c style arrays are passed by reference by default
    void processInput(uint8_t keypad[16], bool &quitFlag);

private:
    SDL_Window* window; // sdl manages the window and renderer internally and returns them by address. so access is only through raw pointers
    SDL_Renderer* renderer;
    int scale;
};

#endif