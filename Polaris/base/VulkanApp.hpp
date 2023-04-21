#pragma once
#ifndef _VULKANAPP_HPP_
#define _VULKANAPP_HPP_

#include "pch.hpp"
#include "VkDebug.hpp"

namespace polaris {

class VulkanApp
{
public:
    VulkanApp(std::string windowName, int initWidth, int initHeight);
    virtual ~VulkanApp();

    float AspectRatio() const;

    void Run();

    virtual void Init();
    virtual void OnResize();
    virtual void UpdateScene(float dt) = 0;
    virtual void DrawScene() = 0;

protected:
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> m_Window{nullptr};
    vk::Instance                                                  m_Instance{nullptr};
    vk::PhysicalDevice                                            m_PhysicalDevice{nullptr};

#ifdef DEBUG
    VkDebug m_Debug;
#endif

    int         m_ClientWidth;
    int         m_ClientHeight;
    std::string m_WndCaption;
    float       m_LastTime{0};
    int         m_FrameCount{0};
    int         m_fps{0};

protected:
    bool InitGLFW();
    bool InitVulkan();
    void CalculateFrameStats();
    void FreeRes();

private:

private:
    bool CreateInstance();
    bool pickPhysicalDevice();
};

}

#endif

