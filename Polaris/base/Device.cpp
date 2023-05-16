#include "Device.hpp"

//=====================================================Debug mode settings=======================================================
//open validationLayer under debug mode


#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

vk::DebugUtilsMessengerEXT debugMessenger{nullptr};
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
    msg << std::string("{") << "Error message: " << pCallbackData->pMessage << std::string("}");

    LOG_ERROR(msg.str());
    
    return VK_FALSE;
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

//=========================================================================================================================


void polaris::Device::OnCreate(std::string appName, std::string engineName, SDL_Window* win){
    CreateInstance(appName, engineName, win);
    if (enableValidationLayers) {
        SetupVulkanDebugMessenger(m_Instance);
    }
    CreateSurface(win);
    PickPhysicalDevice();
    CreateLogicalDevice();
}


void polaris::Device::OnDestroy(){
    if (enableValidationLayers) {
        DestroyVulkanDebugMessenger(m_Instance);
    }
    m_Device.destroy();
    m_Instance.destroySurfaceKHR(m_Surface);
    m_Instance.destroy();
}


bool polaris::Device::CreateInstance(std::string appName, std::string engineName, SDL_Window* win){
    try {
        vk::ApplicationInfo appInfo{};
        appInfo.sType = vk::StructureType::eApplicationInfo;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        vk::InstanceCreateInfo createInfo;
        createInfo.sType = vk::StructureType::eInstanceCreateInfo;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t sdlExtensionCount = 0;
        if (!SDL_Vulkan_GetInstanceExtensions(win, &sdlExtensionCount, nullptr)) {
            throw std::exception("Can't get SDL extension count!"); 
        }
        std::vector<const char*> sdlExtensions(sdlExtensionCount);
        if (!SDL_Vulkan_GetInstanceExtensions(win, &sdlExtensionCount, sdlExtensions.data())) {
            throw std::exception("Can't get SDL extensions!"); 
        }
        std::vector<const char*> extensions(sdlExtensions.begin(), sdlExtensions.end());
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }      
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        m_InstanceProperties.AddExtensions(extensions);

        if (enableValidationLayers) {
            const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
            uint32_t layerCount;
            vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
            std::vector<vk::LayerProperties> availableLayers(layerCount);
            vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
            
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
                std::vector<const char*> enabledLayers{validationLayerName};
                m_InstanceProperties.AddLayers(enabledLayers);
            }
            else {
                throw std::exception("Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled");
            }
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

    m_Instance = vk::createInstance(createInfo);
    if (!m_Instance) {throw std::exception("Create Instance Failed!");}
    } 
    catch (vk::SystemError& err) {
         LOG_ERROR(err.what());
         return false;
    }
    catch (std::exception& err){
        LOG_ERROR(err.what());
        return false;
    }
    catch (...){
        LOG_ERROR("Unknow error!");
        return false;
    }
    return true;
}


bool polaris::Device::CreateSurface(SDL_Window* win){
    try{
        VkSurfaceKHR temSurface;
        auto res = SDL_Vulkan_CreateSurface(win, m_Instance, &temSurface);
        if (res != SDL_TRUE) {
            throw std::exception("Failed to create surface!");
        }
        m_Surface = temSurface;
    }
    catch (std::exception& e){
        LOG_ERROR(e.what());
        return false;
    }
    
    return true;
}


bool polaris::Device::PickPhysicalDevice(){
    try {
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
        if (!m_PhysicalDevice) {throw std::exception("Create PhysicalDevice Failed!");}
    }
    catch (vk::SystemError& err) {
         LOG_ERROR(err.what());
         return false;
    }
    catch (std::exception& err){
        LOG_ERROR(err.what());
        return false;
    }
    catch (...){
        LOG_ERROR("Unknow error!");
        return false;
    }
    return true;
}


uint32_t polaris::Device::FindQueueFamilies(){
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


bool polaris::Device::CreateLogicalDevice(){
    try {
        uint32_t queueFamilyIndices = FindQueueFamilies();
        vk::DeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
        queueCreateInfo.setQueueFamilyIndex(queueFamilyIndices);
        queueCreateInfo.setQueueCount(1);
        float queuePriority = 1.0f;
        queueCreateInfo.setPQueuePriorities(&queuePriority);

        m_Device = m_PhysicalDevice.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), queueCreateInfo));
        m_GraphicsQueue = m_Device.getQueue(queueFamilyIndices, 0);

        if (! (m_Device && m_GraphicsQueue)) {throw std::exception("Create Logical Device Failed!");}
    }
    catch (vk::SystemError& err) {
         LOG_ERROR(err.what());
         return false;
    }
    catch (std::exception& err){
        LOG_ERROR(err.what());
        return false;
    }
    catch (...){
        LOG_ERROR("Unknow error!");
        return false;
    }
    return true;
}
