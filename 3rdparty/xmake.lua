package("tbb")
    add_deps("cmake")
    set_sourcedir(path.join("$(projectdir)/3rdparty", "oneTBB"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

package("vulkan")
    add_deps("cmake")
    set_sourcedir(path.join("$(projectdir)/3rdparty", "vulkan-1.3.243.0"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs,"-DCMAKE_INSTALL_INCLUDEDIR=" .. package:installdir())
        table.insert(configs,"-DCMAKE_INSTALL_LIBDIR=" .. package:installdir())
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

package("glfw")
    add_deps("cmake")
    set_sourcedir(path.join("$(projectdir)/3rdparty", "glfw-3.3.8"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()


add_requires("glfw")
add_requires("vulkan")

target("imgui")
    set_default(false)
    set_kind("static")
    set_languages("cxx11")
    add_files("imgui/*.cpp")
    add_headerfiles("imgui/*.h")
    add_includedirs("imgui", {public = true})

    add_files("imgui/backends/imgui_impl_glfw.cpp")
    add_headerfiles("imgui/backends/imgui_impl_glfw.h")
    add_packages("glfw")

    add_files("imgui/backends/imgui_impl_vulkan.cpp")
    add_headerfiles("imgui/backends/imgui_impl_vulkan.h")
    add_packages("vulkan")


target("lualib")
    set_kind("static")
    set_default(false)
    add_files("lua-5.4.4/src/*.c")
    remove_files("lua-5.4.4/src/lua.c")
    remove_files("lua-5.4.4/src/luac.c")
    add_includedirs("lua-5.4.4/src", {public = true})

target("luabridge")
    set_kind("headeronly")
    set_default(false)
    add_includedirs("Luabridge/Source", {public = true})
