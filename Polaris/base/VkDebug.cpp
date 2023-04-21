#include "VkDebug.hpp"

PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator,
                                                            VkDebugUtilsMessengerEXT* pMessager)
{
    return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessager);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                            VkDebugUtilsMessengerEXT messager,
                                                            const VkAllocationCallbacks* pAllocator)
                                                            
{
    return pfnVkDestroyDebugUtilsMessengerEXT(instance, messager, pAllocator);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        ){
    std::ostringstream msg;
    msg << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << ": "
        << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageType)) << ":\n";
    msg << std::string("\t") << "messageIDName   = " << pCallbackData->pMessageIdName << "\n";
    msg << std::string("\t") << "messageIDNumber = " << pCallbackData->messageIdNumber << "\n";
    msg << std::string("\t") << "message         = " << pCallbackData->pMessage << "\n";
    
    throw std::runtime_error(msg.str());
}

void polaris::VkDebug::create(const vk::Instance& instance){
    try {
        pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>
                                            (instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
        if (!pfnVkCreateDebugUtilsMessengerEXT) {
            throw std::exception("GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");
        }
        pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>
                                            (instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
        if (!pfnVkDestroyDebugUtilsMessengerEXT) {
            throw std::exception("GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");
        }
        
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlages(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
        vk::DebugUtilsMessengerCreateInfoEXT createInfo = {};

        createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
        createInfo.setMessageSeverity(severityFlages);
        createInfo.setMessageType(messageTypeFlags);
        createInfo.setPfnUserCallback(&debugCallback);
        createInfo.setPUserData(nullptr);
        m_DebugMessenger = instance.createDebugUtilsMessengerEXT(createInfo);
    }
    catch (vk::SystemError & err){
        LOG_ERROR(err.what());        
    }
    catch (std::exception & err)
    {
        LOG_ERROR(err.what());
    }
    catch (...)
    {
        LOG_ERROR("unknown error!");
    }
}

void polaris::VkDebug::destroy(const vk::Instance& instance){
    instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger);
}
