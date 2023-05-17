add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

includes("3rdparty")

add_requires("tbb")
add_requires("lua")
add_requires("luabridge")
add_requires("assimp")
add_requires("vulkan-hpp")
add_requires("spdlog")
add_requires("sdl")

target("Polaris")
    set_kind("static")
    set_pcxxheader("base/pch.hpp")
    add_files("./base/*.cpp")
    add_headerfiles("./**.hpp")
    add_includedirs(".", {public = true})


    set_languages("cxx20")
    if is_mode("debug") then
        add_defines("DEBUG")
    end
    if is_mode("release") then
        add_defines("NDEBUG")
    end

    add_packages("tbb", {public = true})
    add_packages("vulkan-hpp", {public = true})
    add_packages("lua", {public = true})
    add_packages("luabridge", {public = true})
    add_packages("assimp", {public=true})
    add_packages("spdlog", {public = true})
    add_packages("sdl", {public = true})
    add_deps("3rdparty", {public = true})
    add_links("vulkan-1")
