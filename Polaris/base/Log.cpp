#include "Log.hpp"

std::shared_ptr<spdlog::logger> polaris::Log::m_Logger;

void polaris::Log::Init(){
    spdlog::set_pattern("%^[%n--%T] %v%$");
    m_Logger = spdlog::stdout_color_mt("Polaris");
    m_Logger->set_level(spdlog::level::trace);
}
