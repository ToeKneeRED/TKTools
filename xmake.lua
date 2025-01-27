set_project("TKTools")

set_languages("c11", "cxx23")
set_policy("build.ccache", false)
set_policy("package.requires_lock", false)

if(is_plat("windows")) then
    add_cxflags("/bigobj")
    set_arch("x64")

    -- :(
    add_syslinks("kernel32")
    add_syslinks("user32")
    add_syslinks("Gdi32")
    add_syslinks("Shell32")

    add_ldflags("-subsystem:windows", {force = true})
end

add_rules("mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

add_requires("spdlog v1.13.0")
add_requireconfs("spdlog", {configs = {wchar = true}})

if is_mode("debug") then
    add_defines("DEBUG")
end

target("Tools")
    set_kind("static")

    add_packages("spdlog", {public = true, wchar = true})

    add_headerfiles("include/*.h")
    -- add_files("src/*.cpp")