#pragma once
#ifndef _TOYRENDER_HPP_
#define _TOYRENDER_HPP_

#include <Polaris.hpp>

class ToyRender : public polaris::VulkanApp
{
public:
    ToyRender():VulkanApp("window", 1920, 1080){}
    void Init(){
        polaris::Log::Init();
        try {
            VulkanApp::Init();
        } catch (const std::exception& e) {
            LOG_ERROR(e.what());
        }
    };
    void DrawScene(){};
    void UpdateScene(float dt){};
    void OnResize(){};
};

#endif
