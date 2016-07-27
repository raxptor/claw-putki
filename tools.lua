solution "Tools"

    configurations {"Release", "Debug"}

    flags { "Symbols" }

    location "build"
    targetdir "build"

    defines {"_CRT_SECURE_NO_WARNINGS"}

    defines("BUILDER_DEFAULT_RUNTIME=x64")

    defines("LIVEUPDATE_ENABLE")
    defines("PUTKI_ENABLE_LOG")
    defines("KOSMOS_ENABLE_LOG")

    if os.get() == "windows" then
        flags {"StaticRuntime"}
    end

    configuration {"linux", "gmake"}
        buildoptions {"-fPIC"}
        buildoptions ("-std=c++11")

    configuration "Debug"
        defines {"DEBUG"}
    configuration "Release"
        flags {"Optimize"}

    configuration {}

    ------------------------------------
    -- Putki must always come first   --
    ------------------------------------

    dofile "ext/putki/builder/premake.lua"
    dofile "ext/kosmos/libs.lua"
    dofile "ext/ccg-ui/libs.lua"

    project "claw-putki-lib"
        language "C++"
        targetname "claw-putki-lib"
        kind "StaticLib"

        -- putki last here
        kosmos_use_builder_lib()
        ccgui_use_builder_lib()
        putki_use_builder_lib()

        putki_typedefs_builder("src/types", true)

    project "claw-databuilder"

        kind "ConsoleApp"
        language "C++"
        targetname "claw-databuilder"

        files { "src/putki/builder-main.cpp" }
        files { "src/builder/**.cpp" }
        links { "claw-putki-lib" }
        includedirs { "src" }

        ccgui_use_builder_lib()
        kosmos_use_builder_lib()
        putki_use_builder_lib()
        putki_typedefs_builder("src/types", false)
