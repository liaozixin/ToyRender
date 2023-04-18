#include "VulkanApp.hpp"


VulkanApp::VulkanApp(std::string windowName, int initWidth, int initHeight)
    :m_WndCaption(windowName),
    m_ClientWidth(initWidth),
    m_ClientHeight(initHeight)
{


}


VulkanApp:: ~VulkanApp()
{
    FreeRes();
}


void VulkanApp:: Init()
{
    if (!InitGLFW()) {
        throw std::runtime_error("Failed to init GLFW!");
    }
    if (!InitVulkan()) {
        throw std::runtime_error("Failed to init vulkan!");
    }
}

void VulkanApp::OnResize()
{

}

bool VulkanApp::InitGLFW(){
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

void VulkanApp::Run(){
    while (!glfwWindowShouldClose(m_Window.get())) {
        glfwSwapBuffers(m_Window.get());
        glfwPollEvents();
        CalculateFrameStats();
        UpdateScene(0.1f);
        DrawScene();
    }
}

void VulkanApp::CalculateFrameStats(){
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - m_LastTime;
    m_FrameCount++;
    if (deltaTime >= 1.0) {
        m_fps = m_FrameCount / deltaTime;
        m_FrameCount = 0;
        m_LastTime = currentTime;
    }
}

void VulkanApp::FreeRes(){
    m_Vkinstance.destroy();
    glfwTerminate();
}

bool VulkanApp::InitVulkan()
{
    if (!CreateInstance()) {
        return false;
    }    
    return true;
}

bool VulkanApp::CreateInstance()
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

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;
    // m_Vkinstance = vk::createInstance(createInfo);
    
        m_Vkinstance = vk::createInstance(createInfo);
    } 
    catch (const vk::SystemError& err) {
        std::cerr << "vk::SystemError: " << err.what() << std::endl;
        return false;
    }
    catch (const std::exception& err) {
        std::cerr << "std::exception: " << err.what() << std::endl;
        return false;

    }
    catch (...){
        std::cerr << "unknown error" << std::endl;
        return false;
    }

    return true;
}
