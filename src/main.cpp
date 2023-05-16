#include "Polaris.hpp"

int main(int argc, char** argv)
{

    std::unique_ptr<polaris::FrameworkWindows> frame = std::make_unique<polaris::FrameworkWindows>("hello");
    return polaris::RunFramework(frame);
}
