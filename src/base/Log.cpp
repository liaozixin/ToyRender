#include "Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> polaris::Log::m_Logger;

void polaris::Log::Init(){
    spdlog::set_pattern("%^[%T] %n: %v%$");
    m_Logger = spdlog::stdout_color_mt("Polaris");
    m_Logger->set_level(spdlog::level::trace);
}
