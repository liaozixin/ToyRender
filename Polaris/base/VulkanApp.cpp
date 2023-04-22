#include "VulkanApp.hpp"

#ifdef DEBUG
    bool enableValidationLayers = true;
    vk::DebugUtilsMessengerEXT debugMessenger{nullptr};

    PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;
#else
    bool enableValidationLayers = false;
#endif

// ==========================================================================================================================

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

    LOG_ERROR(msg.str());
    
    throw std::runtime_error(msg.str());
}

void SetupVulkanDebugMessenger(vk::Instance& instance)
{
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
        debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo);
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


void DestroyVulkanDebugMessenger(vk::Instance& instance){
    instance.destroyDebugUtilsMessengerEXT(debugMessenger);
}
// ==========================================================================================================================


polaris::VulkanApp::VulkanApp(std::string windowName, int initWidth, int initHeight)
    :m_WndCaption(windowName),
    m_ClientWidth(initWidth),
    m_ClientHeight(initHeight)
{}


polaris::VulkanApp:: ~VulkanApp(){
    FreeRes();
}


void polaris::VulkanApp:: Init(){
    if (!InitGLFW()) {
        throw std::runtime_error("Failed to init GLFW!");
    }
    if (!InitVulkan()) {
        throw std::runtime_error("Failed to init vulkan!");
    }
}

void polaris::VulkanApp::OnResize(){

}

bool polaris::VulkanApp::InitGLFW(){
    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = std::move(std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>(
                    glfwCreateWindow(m_ClientWidth, m_ClientHeight, m_WndCaption.c_str(), nullptr, nullptr), 
                                    [](GLFWwindow* ptr)
                                    {
                                        glfwDestroyWindow(ptr);
                                        glfwTerminate();
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



bool polaris::VulkanApp::InitVulkan()
{
    try {
        CreateInstance();
#ifdef DEBUG
        SetupVulkanDebugMessenger(m_Instance);
#endif
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
    }
    catch ( vk::SystemError & err )
    {
        LOG_ERROR(err.what());
        return false;
    }
    catch ( std::exception & err )
    {
        LOG_ERROR(err.what());
        return false;
    }
    catch ( ... )
    {
        LOG_ERROR("unknown error");
        return false;
    }
    return true;
}

bool polaris::VulkanApp::CreateInstance()
{
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
    m_Instance = vk::createInstance(createInfo);
    if (m_Instance) {
        return true;
    }
    return false;
}


bool polaris::VulkanApp::CreateSurface(){
    VkSurfaceKHR temSurface;
    auto res = glfwCreateWindowSurface(m_Instance, m_Window.get(), nullptr, &temSurface);
    if (res != VK_SUCCESS) {
        throw std::exception("Failed to create surface!");
        return false;
    }
    m_Surface = temSurface;
    return true;
}

bool polaris::VulkanApp::PickPhysicalDevice(){
    std::vector<vk::PhysicalDevice> physicalDevices = m_Instance.enumeratePhysicalDevices();
    if (physicalDevices.size() == 0) {
        throw std::exception("Failed to find GPUs with Vulkan support!");
        return false;
    }

    for (auto device : physicalDevices) {
        vk::PhysicalDeviceProperties deviceProperties;
        vk::PhysicalDeviceFeatures deviceFeatures;
        deviceProperties = device.getProperties();
        deviceFeatures = device.getFeatures();
        if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            m_PhysicalDevice = device;
        }
    }
    LOG_INFO("{} {}", "Choose GPU: ", m_PhysicalDevice.getProperties().deviceName);
    if (m_PhysicalDevice) {
        return true;
    }
    return false;
}

uint32_t polaris::VulkanApp::FindQueueFamilies(){
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();
    auto propertyIterator = std::find_if(queueFamilyProperties.begin(),
                                        queueFamilyProperties.end(),
                                        [](const vk::QueueFamilyProperties&  qfp){
                                            return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
                                        }); 
    size_t graphicsQueueFamilyIndex = std::distance(queueFamilyProperties.begin(), propertyIterator);
    assert(graphicsQueueFamilyIndex < queueFamilyProperties.size());
    return static_cast<uint32_t>(graphicsQueueFamilyIndex);
}

bool polaris::VulkanApp::CreateLogicalDevice(){
    uint32_t queueFamilyIndices = FindQueueFamilies();
    vk::DeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
    queueCreateInfo.setQueueFamilyIndex(queueFamilyIndices);
    queueCreateInfo.setQueueCount(1);
    float queuePriority = 1.0f;
    queueCreateInfo.setPQueuePriorities(&queuePriority);

    m_Device = m_PhysicalDevice.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), queueCreateInfo));
    m_GraphicsQueue = m_Device.getQueue(queueFamilyIndices, 0);

    if (m_Device && m_GraphicsQueue) {
        return true;
    }
    return false;
}

void polaris::VulkanApp::FreeRes(){
#ifdef DEBUG
    DestroyVulkanDebugMessenger(m_Instance);
#endif
    m_Device.destroy();
    m_Instance.destroySurfaceKHR(m_Surface);
    m_Instance.destroy();
}
