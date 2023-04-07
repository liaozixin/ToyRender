#include <iostream>
#include <vector>
#include <tbb/tbb.h>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

using namespace std;

int main(int argc, char** argv)
{
    std::vector<int> v{1, 2, 4, 5, 6, 7};

    tbb::parallel_for_each(v.begin(), v.end(),
            [](int& v){
                v += 1;
            });

    for (auto x : v) {
        std::cout << x << std::endl;
    }

    return 0;
}
