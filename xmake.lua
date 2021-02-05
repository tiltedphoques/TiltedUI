set_languages("cxx17")

set_xmakever("2.5.1")

add_requires("tiltedcore", "mimalloc", "hopscotch-map", {configs = {rltgenrandom = true }})
add_requires("catch2")

add_rules("mode.debug","mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

if is_mode("release") then
    add_ldflags("/LTCG", "/OPT:REF")
    add_cxflags("/Ot", "/GL", "/Ob2", "/Oi", "/GS-")
    add_defines("NDEBUG")
    set_optimize("fastest")
end

target("TiltedUi")
    set_kind("static")
    set_group("Libraries")
    add_files("Code/ui/src/*.cpp")
    add_includedirs(
        "Code/ui/include/", 
        "ThirdParty/DirectXTK/include/", 
        "ThirdParty/CEF/", 
        directx_dir .. "/Include", {public = true})
    add_headerfiles("Code/ui/include/*.hpp", {prefixdir = "TiltedUi"})
    add_syslinks(
        "dxguid.lib",
        "d3d9.lib",
        "d3d11.lib",
        "d3dx9.lib")
    add_packages("tiltedcore", "mimalloc", "hopscotch-map")
    add_linkdirs(
        directx_dir .. "/Lib/x64",
        "$(VsInstallDir)DIA SDK/lib/amd64",
        "/ThirdParty/DirectXTK/lib/x64",
        "/ThirdParty/CEF/lib/Win64"
    )
    if is_mode("debug") then
        add_links(
            "libcef_d",
            "libcef_dll_wrapper_d",
            "DirectXTK_d")
    elseif is_mode("release") or is_mode("releasedebug") then
        add_links(
            "libcef_r",
            "libcef_dll_wrapper_r",
            "DirectXTK_r")
    end

target("UiProcess")
    set_kind("static")
    set_group("Libraries")
    add_files("Code/ui_process/src/*.cpp")
    add_headerfiles("Code/ui_process/include/*.hpp", {prefixdir = "UiProcess"})
    add_includedirs(
        "Code/ui_process/include/", 
        "ThirdParty/CEF/", 
        {public = true})
    add_packages("tiltedcore", "mimalloc", "hopscotch-map")
    if is_mode("debug") then
        add_links(
            "libcef_d",
            "libcef_dll_wrapper_d")
    elseif is_mode("release") or is_mode("releasedebug") then
        add_links(
            "libcef_r",
            "libcef_dll_wrapper_r")
    end
    
target("TiltedUi_Tests")
    set_kind("binary")
    set_group("Tests")
    add_files("Code/tests/src/*.cpp")
    add_deps("TiltedUi")
    add_packages("tiltedcore", "catch2", "hopscotch-map")
