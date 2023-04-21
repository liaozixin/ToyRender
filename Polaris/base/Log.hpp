#pragma once
#ifndef _LOG_HPP_
#define _LOG_HPP_

#include "pch.hpp"

namespace polaris {

class Log{
public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetLogger() {return m_Logger;};

private:
    static std::shared_ptr<spdlog::logger> m_Logger;
};



}

#define LOG_TRACE(...)     ::polaris::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)      ::polaris::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)      ::polaris::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)     ::polaris::Log::GetLogger()->error(__VA_ARGS__)

#endif
