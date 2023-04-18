#pragma once
#ifndef _TOYRENDER_HPP_
#define _TOYRENDER_HPP_

#include "base/VulkanApp.hpp"

class ToyRender : public VulkanApp
{
public:
    ToyRender():VulkanApp("window", 640, 480){}
    void Init(){
        try {
            VulkanApp::Init();
        } catch (const std::exception& e) {
            std::cerr<<e.what()<<std::endl;
        }
    };
    void DrawScene(){};
    void UpdateScene(float dt){};
    void OnResize(){};
};

#endif
