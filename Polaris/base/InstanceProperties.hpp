#pragma once
#ifndef _INSTANCEPROPERTIES_HPP_
#define _INSTANCEPROPERTIES_HPP_

#include "pch.hpp"

namespace polaris {

class InstanceProperties{
public:
    void AddLayers(const std::vector<const char*>& layers){
        m_LayersName.assign(layers.begin(), layers.end());
    };
    void AddExtensions(const std::vector<const char*>& extensions){
        m_ExtensionsName.assign(extensions.begin(), extensions.end());
    }
    void PrintLayersName(){
        LOG_INFO("Layers' name:");
        for (const auto e : m_LayersName) {
            LOG_INFO("{}", e);
        }
    } 
    void PrintExtensionsName(){
        LOG_INFO("Extensions' name:");
        for (const auto e : m_ExtensionsName) {
            LOG_INFO("{}", e);
        }
    } 
private:
    std::vector<const char*> m_LayersName;
    std::vector<const char*> m_ExtensionsName;
};

}



#endif
