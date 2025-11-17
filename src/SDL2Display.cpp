#include "SDL2Display.hpp"
#include <iostream>

/*

This file is supposed to do exactly 3 things:

Initialise sdl2 window and renderer (sdl2 constructor and destructor)
Draw the chip8 64x32 framebuffer scaled-up (draw())
convert real keyboard input to chip8 keypad value (processInput())

*/

SDL2Display::SDL2Display(int windowScale) {

    scale = windowScale;

    //initialise sdl2 and if error occurs then return failure
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) !=0){
        std::cout<< "Error occured during SDL2 initialisation" << SDL_GetError() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow(
        "Chip8 emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        64*scale,
        32*scale,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cout<< "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        std::exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        std::cout<< "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        std::exit(EXIT_FAILURE);
    }

}

SDL2Display::~SDL2Display() {

    //reverse destroy everything
    if(renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if(window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

void SDL2Display:: draw(const uint32_t video[64*32]) {

    //clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //prepare a rectangle for drawing pixels
    SDL_Rect pixel;
    pixel.w = scale;
    pixel.h = scale;

    //loop over all 64x32 pixels to fill the pixel on renderer
    for(int y=0; y<32; y++){
        for(int x=0; x<64; x++){

            int index =  y*64 + x;

            if(video[index]==1) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            pixel.x = x*scale;
            pixel.y = y*scale;

            SDL_RenderFillRect(renderer, &pixel);
        }
    }

    //present final frame
    SDL_RenderPresent(renderer);

}

void SDL2Display::processInput(uint8_t keypad[16], bool &quitFlag) {
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        
        if(event.type==SDL_QUIT){
            quitFlag = true;
            return;
        }

        if(event.type==SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_1: keypad[0x1] = 1; break;
                case SDLK_2: keypad[0x2] = 1; break;
                case SDLK_3: keypad[0x3] = 1; break;
                case SDLK_4: keypad[0xC] = 1; break;

                case SDLK_q: keypad[0x4] = 1; break;
                case SDLK_w: keypad[0x5] = 1; break;
                case SDLK_e: keypad[0x6] = 1; break;
                case SDLK_r: keypad[0xD] = 1; break;

                case SDLK_a: keypad[0x7] = 1; break;
                case SDLK_s: keypad[0x8] = 1; break;
                case SDLK_d: keypad[0x9] = 1; break;
                case SDLK_f: keypad[0xE] = 1; break;

                case SDLK_z: keypad[0xA] = 1; break;
                case SDLK_x: keypad[0x0] = 1; break;
                case SDLK_c: keypad[0xB] = 1; break;
                case SDLK_v: keypad[0xF] = 1; break;

                case SDLK_ESCAPE: quitFlag = true; break; //quit emulator using esc
            }
        }

        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_1: keypad[0x1] = 0; break;
                case SDLK_2: keypad[0x2] = 0; break;
                case SDLK_3: keypad[0x3] = 0; break;
                case SDLK_4: keypad[0xC] = 0; break;

                case SDLK_q: keypad[0x4] = 0; break;
                case SDLK_w: keypad[0x5] = 0; break;
                case SDLK_e: keypad[0x6] = 0; break;
                case SDLK_r: keypad[0xD] = 0; break;

                case SDLK_a: keypad[0x7] = 0; break;
                case SDLK_s: keypad[0x8] = 0; break;
                case SDLK_d: keypad[0x9] = 0; break;
                case SDLK_f: keypad[0xE] = 0; break;

                case SDLK_z: keypad[0xA] = 0; break;
                case SDLK_x: keypad[0x0] = 0; break;
                case SDLK_c: keypad[0xB] = 0; break;
                case SDLK_v: keypad[0xF] = 0; break;
            }
        }
    }
}
