#include "FrameworkWindow.hpp"

int polaris::RunFramework(std::unique_ptr<polaris::FrameworkWindows>& pFramework){
    SDL_Init(SDL_INIT_EVERYTHING);
    Log::Init();

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    std::get<0>(pFramework->GetSysInfo().m_DesktopRes) = dm.w;
    std::get<1>(pFramework->GetSysInfo().m_DesktopRes) = dm.h;

    auto win = SDL_CreateWindow("hello", 
                                    SDL_WINDOWPOS_CENTERED, 
                                    SDL_WINDOWPOS_CENTERED, 
                                    static_cast<uint32_t>(dm.w * 0.7),
                                    static_cast<uint32_t>(dm.h * 0.7),
                                    SDL_WINDOW_SHOWN | 
                                    SDL_WINDOW_VULKAN);
    SDL_Event event;
    bool isQuit = false;

    pFramework->DeviceInit(win);

    while (!isQuit) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isQuit = true;
                    break;
            }
        }
    }
    
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

polaris::FrameworkWindows::FrameworkWindows(std::string title)
    :m_Title(title)
{}

void polaris::FrameworkWindows::DeviceInit(SDL_Window* win){
    m_Device.OnCreate(m_Title, "No Engine", win);

}
