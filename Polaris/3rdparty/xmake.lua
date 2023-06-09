package("tbb")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "oneTBB"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

package("sdl")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "SDL-2.26.5"))
    if is_plat("windows") then
        add_syslinks("Winmm","Ole32","CfgMgr32","Setupapi","Version","Imm32","OleAut32", "User32", "Shell32", "Gdi32")
    end
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()


package("assimp")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "assimp-5.2.5"))
    if is_plat("windows") then
        add_syslinks("advapi32")
    end
    on_install(function (package)
        local configs = {"-DASSIMP_BUILD_SAMPLES=OFF",
                                "-DASSIMP_BUILD_TESTS=OFF",
                                "-DASSIMP_BUILD_DOCS=OFF",
                                "-DASSIMP_BUILD_FRAMEWORK=OFF",
                                "-DASSIMP_INSTALL_PDB=ON",
                                "-DASSIMP_INJECT_DEBUG_POSTFIX=ON",
                                "-DASSIMP_BUILD_ZLIB=OFF",
                                "-DSYSTEM_IRRXML=ON",
                                "-DASSIMP_WARNINGS_AS_ERRORS=OFF"}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))

        local function add_config_arg(config_name, cmake_name)
            table.insert(configs, "-D" .. cmake_name .. "=" .. (package:config(config_name) and "ON" or "OFF"))
        end
        add_config_arg("shared",           "BUILD_SHARED_LIBS")
        add_config_arg("double_precision", "ASSIMP_DOUBLE_PRECISION")
        add_config_arg("no_export",        "ASSIMP_NO_EXPORT")
        add_config_arg("asan",             "ASSIMP_ASAN")
        add_config_arg("ubsan",            "ASSIMP_UBSAN")

        if package:is_plat("windows", "linux", "macosx", "mingw") then
            add_config_arg("build_tools", "ASSIMP_BUILD_ASSIMP_TOOLS")
        else
            table.insert(configs, "-DASSIMP_BUILD_ASSIMP_TOOLS=OFF")
        end

        import("package.tools.cmake").install(package, configs)
                -- copy pdb
        if package:is_plat("windows") then
            if package:config("shared") then
                os.trycp(path.join(package:buildir(), "bin", "**.pdb"), package:installdir("bin"))
            else
                os.trycp(path.join(package:buildir(), "lib", "**.pdb"), package:installdir("lib"))
            end
        end
    end)
package_end()

package("lua")
    set_sourcedir(path.join(os.scriptdir(), "lua-5.4.4"))
    on_install(function (package)
        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)
package_end()

package("luabridge")
    set_sourcedir(path.join(os.scriptdir(), "LuaBridge"))
    on_install(function (package)
        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)
package_end()


-- add_requires("sdl")
-- add_requires("vulkan-hpp")
-- add_rules("mode.debug", "mode.release")
-- target("imgui")
--     set_default(false)
--     set_kind("static")
--     set_languages("cxx11")
--
--     before_build(function (target)
--         if not os.exists(path.join(os.scriptdir(), "include", "imgui")) then
--             os.cp(path.join(os.scriptdir(), "imgui", "*.h"), path.join(os.scriptdir(), "include", "imgui")..path.translate("/"))
--             os.cp(path.join(os.scriptdir(), "imgui", "backends", "imgui_impl_glfw.h"), path.join(os.scriptdir(), "include", "imgui")..path.translate("/"))
--             os.cp(path.join(os.scriptdir(), "imgui", "backends", "imgui_impl_vulkan.h"), path.join(os.scriptdir(), "include", "imgui")..path.translate("/"))
--         end
--         if not os.exists(path.join(os.scriptdir(), "lib", "vulkan-1.lib")) then
--             os.cp(path.join(os.getenv("VK_SDK_PATH"), "Lib", "vulkan-1.lib"), path.join(os.scriptdir(), "lib")..path.translate("/"))
--         end
--     end)
--
--     add_files("imgui/*.cpp")
--     add_files("imgui/backends/imgui_impl_glfw.cpp")
--     add_files("imgui/backends/imgui_impl_vulkan.cpp")
--     add_packages("glfw")
--     add_packages("vulkan-hpp")
--     add_links("vulkan-1")
--     add_linkdirs("lib")
--     add_includedirs("imgui")

target("3rdparty")
    set_kind("phony")
    before_build(function (target)
        if not os.exists(os.scriptdir(), "include", "glm") then
            os.cp(path.join(os.getenv("VK_SDK_PATH"), "Include", "glm"), path.join(os.scriptdir(), "include", "glm"), 
                    {rootdir = path.join(os.getenv("VK_SDK_PATH"), "Include", "glm")})
        end
        if not os.exists(path.join(os.scriptdir(), "lib", "vulkan-1.lib")) then
            os.cp(path.join(os.getenv("VK_SDK_PATH"), "Lib", "vulkan-1.lib"), path.join(os.scriptdir(), "lib")..path.translate("/"))
        end
    end)
    add_includedirs(path.join(os.scriptdir(), "include"), {public=true})
    add_linkdirs(path.join(os.scriptdir(), "lib"), {public=true})



