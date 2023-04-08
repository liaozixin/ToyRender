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
    if is_plat("windows") then
        add_syslinks("user32", "shell32", "gdi32")
    end
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()


package("lua")
    set_sourcedir(path.join("$(projectdir)/3rdparty", "lua-5.4.4"))
    on_install(function (package)
        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)
package_end()

package("luabridge")
    set_sourcedir(path.join("$(projectdir)/3rdparty", "LuaBridge"))
    on_install(function (package)
        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)
package_end()

add_requires("glfw")
add_requires("vulkan")

target("imgui")
    set_default(false)
    set_kind("static")
    set_languages("cxx11")

    before_build(function (target)
        os.cp("$(scriptdir)/imgui/*.h", "$(scriptdir)/include/")
        os.cp("$(scriptdir)/imgui/backends/imgui_impl_glfw.h", "$(scriptdir)/include/")
        os.cp("$(scriptdir)/imgui/backends/imgui_impl_vulkan.h", "$(scriptdir)/include/")
    end)

    add_includedirs("$(scriptdir)/include", {public=true})
    add_includedirs("imgui")
    add_files("imgui/*.cpp")
    add_files("imgui/backends/imgui_impl_glfw.cpp")
    add_files("imgui/backends/imgui_impl_vulkan.cpp")
    add_packages("glfw")
    add_packages("vulkan")

target("3rdparty")
    set_kind("phony")
    add_deps("imgui")
    add_includedirs("include", {public=true})



