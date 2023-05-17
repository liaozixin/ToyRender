#pragma once
#ifndef _PCH_HPP_
#define _PCH_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>
#include <tuple>
#include <optional>
#include <set>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "Log.hpp"

#endif
