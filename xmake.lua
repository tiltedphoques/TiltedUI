set_languages("cxx17")
set_xmakever("2.5.1")

add_requires(
    "tiltedcore",
    "catch2",
    "mimalloc",
    "directxtk",
    "glm",
    "cef",
    "hopscotch-map")

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
        directx_dir .. "/Include", {public = true})
    add_headerfiles("Code/ui/include/*.hpp", {prefixdir = "TiltedUi"})
    add_syslinks("dxguid", "d3d11")
    add_packages(
        "tiltedcore",
        "cef",
        "glm",
        "directxtk",
        "mimalloc",
        "hopscotch-map")
    add_linkdirs(directx_dir .. "/Lib/x64")

target("UiProcess")
    set_kind("static")
    set_group("Libraries")
    add_files("Code/ui_process/src/*.cpp")
    add_headerfiles("Code/ui_process/include/*.hpp", {prefixdir = "UiProcess"})
    add_includedirs(
        "Code/ui_process/include/",
        {public = true})
    add_packages(
        "tiltedcore",
        "cef",
        "mimalloc",
        "hopscotch-map")

--target("TiltedUi_Tests")
--    set_kind("binary")
--    set_group("Tests")
--    add_files("Code/tests/src/*.cpp")
--    add_deps("TiltedUi")
--    add_packages("tiltedcore", "catch2", "hopscotch-map")
