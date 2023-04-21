#include "VulkanApp.hpp"

#ifdef DEBUG
    bool enableValidationLayers = true;
#else
    bool enableValidationLayers = false;
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
    glfwTerminate();
#ifdef DEBUG
    m_Debug.destroy(m_Instance);
#endif
    m_Instance.destroy();
}

bool polaris::VulkanApp::InitVulkan()
{
    if (!CreateInstance()) {
        return false;
    }
#ifdef DEBUG
        m_Debug.create(m_Instance);
#endif
    if (!pickPhysicalDevice()) {
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
        m_Instance = vk::createInstance(createInfo);
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



bool polaris::VulkanApp::pickPhysicalDevice(){
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
        LOG_INFO("{} {}", vk::to_string(deviceProperties.deviceType), deviceProperties.deviceName);
    }
    LOG_INFO("{} {}", "Choose GPU: ", m_PhysicalDevice.getProperties().deviceName);
    return true;
}
