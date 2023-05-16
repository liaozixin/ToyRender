#pragma once
#ifndef _DEVICE_HPP_
#define _DEVICE_HPP_
#include "pch.hpp"
#include "InstanceProperties.hpp"

namespace polaris {

class Device{
public:
    Device(){};
    ~Device(){};

    void OnCreate(std::string appName, std::string engineName, SDL_Window* win);
    void OnDestroy();

protected:
    vk::Instance       m_Instance{nullptr};
    InstanceProperties m_InstanceProperties;
    vk::SurfaceKHR     m_Surface{nullptr};
    vk::PhysicalDevice m_PhysicalDevice{nullptr};
    vk::Device         m_Device{nullptr};
    vk::Queue          m_GraphicsQueue{nullptr};

private:
    bool CreateInstance(std::string appName, std::string engineName, SDL_Window* win);
    bool CreateSurface(SDL_Window* win);
    bool PickPhysicalDevice();
    uint32_t FindQueueFamilies();
    bool CreateLogicalDevice();
};

}


#endif
