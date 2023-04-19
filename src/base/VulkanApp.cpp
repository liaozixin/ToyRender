#include "VulkanApp.hpp"
#include <algorithm>
#include <sstream>
#include <vulkan/vulkan_to_string.hpp>


#ifdef DEBUG
    const bool enableValidationLayers = true;
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
#else
    const bool enableValidationLayers = false;
#endif


polaris::VulkanApp::VulkanApp(std::string windowName, int initWidth, int initHeight)
    :m_WndCaption(windowName),
    m_ClientWidth(initWidth),
    m_ClientHeight(initHeight)
{}


polaris::VulkanApp:: ~VulkanApp()
{
    FreeRes();
}


void polaris::VulkanApp:: Init()
{
    if (!InitGLFW()) {
        throw std::runtime_error("Failed to init GLFW!");
    }
    if (!InitVulkan()) {
        throw std::runtime_error("Failed to init vulkan!");
    }
}

void polaris::VulkanApp::OnResize()
{

}

bool polaris::VulkanApp::InitGLFW(){
    if (!glfwInit()) {
        return false;
    }

    m_Window = std::move(std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>(
                    glfwCreateWindow(m_ClientWidth, m_ClientHeight, m_WndCaption.c_str(), nullptr, nullptr), 
                                    [](GLFWwindow* ptr)
                                    {
                                        glfwDestroyWindow(ptr);
                                    }));
    
    if (!m_Window) {
        glfwTerminate();
        return false;
    }
    int x, y;
    glfwGetFramebufferSize(m_Window.get(), &x, &y);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    glfwSetWindowPos(m_Window.get(), (mode->width - x) / 2, (mode->height - y) / 2);
    glfwSwapInterval(1);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    return true;
}

void polaris::VulkanApp::Run(){
    while (!glfwWindowShouldClose(m_Window.get())) {
        glfwSwapBuffers(m_Window.get());
        glfwPollEvents();
        CalculateFrameStats();
        UpdateScene(0.1f);
        DrawScene();
    }
}

void polaris::VulkanApp::CalculateFrameStats(){
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - m_LastTime;
    m_FrameCount++;
    if (deltaTime >= 1.0) {
        m_fps = m_FrameCount / deltaTime;
        m_FrameCount = 0;
        m_LastTime = currentTime;
    }
}

void polaris::VulkanApp::FreeRes(){
#ifdef DEBUG
    m_Vkinstance.destroyDebugUtilsMessengerEXT(m_debugUtilsMessager);
#endif
    m_Vkinstance.destroy();
    glfwTerminate();
}

bool polaris::VulkanApp::InitVulkan()
{
    if (!CreateInstance()) {
        return false;
    }
    if (!setupDebugCallback()) {
        return false;
    }
    return true;
}

bool polaris::VulkanApp::CreateInstance()
{
    try {
        vk::ApplicationInfo appInfo{};
        appInfo.sType = vk::StructureType::eApplicationInfo;
        appInfo.pApplicationName = m_WndCaption.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        vk::InstanceCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eInstanceCreateInfo;
        createInfo.pApplicationInfo = &appInfo;
        
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        
        if (enableValidationLayers) {
            const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
            uint32_t layerCount;
            vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
            std::vector<vk::LayerProperties> availableLayers(layerCount);
            vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
            
            bool layerFound = false;
            auto res = std::find_if(availableLayers.begin(), availableLayers.end(), 
                        [validationLayerName](vk::LayerProperties another){
                            if (std::strcmp(validationLayerName, another.layerName) == 0) {
                                return true;
                            }
                            else {
                                return false;
                            }
                        });
            if (res != availableLayers.end()) {
                createInfo.ppEnabledLayerNames = &validationLayerName;
                createInfo.enabledLayerCount = 1;
            }
            else {
                throw std::exception("Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled");
            }
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        
        m_Vkinstance = vk::createInstance(createInfo);
    
    } 
    catch (const vk::SystemError& err) {
        LOG_ERROR(err.what());
        return false;
    }
    catch (const std::exception& err) {
        LOG_ERROR(err.what());
        return false;
    }
    catch (...){
        LOG_ERROR("unknown error!");
        return false;
    }

    return true;
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

bool polaris::VulkanApp::setupDebugCallback(){
    if (!enableValidationLayers) {
        return true;
    } 
    try{
        pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>
                                            (m_Vkinstance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
        if (!pfnVkCreateDebugUtilsMessengerEXT) {
            throw std::exception("GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");
        }
        pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>
                                            (m_Vkinstance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
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
        createInfo.messageSeverity = severityFlages;
        createInfo.messageType = messageTypeFlags;
        createInfo.pfnUserCallback = &debugCallback;
        createInfo.pUserData = nullptr;
        m_debugUtilsMessager = m_Vkinstance.createDebugUtilsMessengerEXT(createInfo);
    }
    catch (vk::SystemError & err){
        LOG_ERROR(err.what());        
        return false;
    }
    catch (std::exception & err)
    {
        LOG_ERROR(err.what());
        return false;
    }
    catch (...)
    {
        LOG_ERROR("unknown error!");
        return false;
    }
    return true;
}
