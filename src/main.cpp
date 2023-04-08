#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    GLFWwindow* window = glfwCreateWindow(640, 480, "window", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW Window!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    int x, y;
    glfwGetFramebufferSize(window, &x, &y);
    std::cout << x << ", " << y << std::endl;

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    std::cout << mode->width << ", " << mode->height << std::endl;
    glfwSetWindowPos(window, (mode->width - x) / 2, (mode->height - y) / 2);

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
