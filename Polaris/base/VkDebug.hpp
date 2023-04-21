#pragma once
#ifndef _VKDEBUG_HPP_
#define _VKDEBUG_HPP_

#include "pch.hpp"

namespace polaris {

class VkDebug{
public:
    VkDebug() = default;
    void create(const vk::Instance&);
    void destroy(const vk::Instance&);
  
private:
    vk::DebugUtilsMessengerEXT m_DebugMessenger{nullptr};
};
}


#endif
