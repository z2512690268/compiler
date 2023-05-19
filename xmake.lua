set_project("compiler")
set_version("0.0.0")
set_xmakever("2.5.0")

add_rules("mode.debug", "mode.release")

-- change \ to \\ in windows
dir_path_windows = function (dir)
    if is_plat("windows") then
        return dir:gsub("\\", "\\\\")
    else
        return dir
    end
end
-- windows add GLOG_NO_ABBREVIATED_SEVERITIES macro
if is_plat("windows") then
    add_defines("GLOG_NO_ABBREVIATED_SEVERITIES")
    add_defines("PLATFORM_OS=\"windows\"")
    add_defines("PROJECT_ROOT_DIR=\"" .. dir_path_windows(os.projectdir()) .. "\\\\\\\\\"")
elseif is_plat("linux") then
    add_defines("PLATFORM_OS=\"linux\"")
    add_defines("PROJECT_ROOT_DIR=\"" .. os.projectdir() .. "/\"")
    add_syslinks("pthread")
end

-- package gtest
package("gtest")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "thirdparty/googletest"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()
add_requires("gtest")

package("glog")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "thirdparty/glog"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
    add_links("glog")
package_end()
add_requires("glog")

package("gflags")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "thirdparty/gflags"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
    add_links("gflags")
package_end()
add_requires("gflags")

-- set_toolchains("clang")
-- set_languages("cxx17")

target("staticlib")
    set_kind("static")
    add_includedirs("include")
    add_files("src/lib/*.cpp")
    add_files("src/lib/frontend/*.cpp")


target("lex")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/bin/lex1.cpp")
    add_deps("staticlib")
    
target("gram")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/bin/gram.cpp")
    add_deps("staticlib")

target("syntax")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/bin/syntax.cpp")
    add_deps("staticlib")

target("test")
    set_kind("binary")
    add_includedirs("include")
    add_files("test/unit/*/*.cpp")
    add_files("test/testmain.cpp")
    add_packages("gtest")
    add_packages("glog")
    add_packages("gflags")
    add_deps("staticlib")
