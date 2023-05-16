#pragma once
#ifndef _FRAMEWORKWINDOW_HPP_
#define _FRAMEWORKWINDOW_HPP_
#include "pch.hpp"
#include "Device.hpp"

namespace polaris {

struct SystemInfo{
    std::string m_CPUName = "UNAVAILABLE";
    std::string m_GPUName = "UNAVAILABLE";
    std::string m_GAPI    = "UNAVAILABLE";

    std::tuple<uint32_t, uint32_t> m_DesktopRes{0, 0};
};

class FrameworkWindows{

public:
    FrameworkWindows(std::string title);
    virtual ~FrameworkWindows(){};

    // virtual void OnCreate() = 0;
    // virtual void OnDestroy() = 0;
    // virtual void OnRender() = 0;
    // virtual void OnResize(bool resizeRender) = 0;
    // virtual void OnUpdateDisplay() = 0;
    
    void DeviceInit(SDL_Window* win);

    std::string GetName()    const {return m_Title;};
    uint32_t    GetWidth()   const {return m_Width;};
    uint32_t    GetHeight()  const {return m_Height;};

    SystemInfo&  GetSysInfo() {return m_SysInfo;};


protected:
    SystemInfo  m_SysInfo;
    std::string m_Title;
    uint32_t    m_Width;
    uint32_t    m_Height;
    polaris::Device m_Device;
};

int RunFramework(std::unique_ptr<polaris::FrameworkWindows>& pFramework);

}


#endif
