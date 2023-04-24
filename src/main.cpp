#include "ToyRender.hpp"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    
    auto window = SDL_CreateWindow("hello", 
                                    SDL_WINDOWPOS_CENTERED, 
                                    SDL_WINDOWPOS_CENTERED, 
                                    1280, 720, 
                                    SDL_WINDOW_SHOWN);
    SDL_Event event;
    bool isQuit = false;
    while (!isQuit) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isQuit = true;
                    break;
            }
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();

    ToyRender ty;
    ty.Init();
    ty.Run();

    return 0;
}
