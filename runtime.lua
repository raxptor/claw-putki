solution "Claw"

	location "build"
	targetdir "build"

        platforms { "x64" }

	configurations {"Debug", "Release", "Ship"}
	
	configuration {"Debug or Release"}
		defines("LIVEUPDATE_ENABLE")
		defines("PUTKI_ENABLE_LOG")
		defines("KOSMOS_ENABLE_LOG")
		
	configuration {"Ship"}
		defines("SHIP")
		defines("PUTKI_NO_RT_PATH_PREFIX")
		
	configuration {"Release or Ship"}
		flags {"Optimize"}
	
	configuration {}
	
	flags { "Symbols" }
	libdirs {"/usr/lib"}

	dofile("ext/putki/runtime/premake.lua")
	dofile("ext/kosmos/runtime.lua")
	dofile("ext/ccg-ui/runtime.lua")
	
        
project "squirrel-lang"
        kind "StaticLib"
        language "C++"
        targetname "squirrel"
        files { "ext/squirrel-lang/squirrel/*.cpp" }
        includedirs { "ext/squirrel-lang/squirrel", "ext/squirrel-lang/include" }

project "claw-runtime"
        kind "ConsoleApp"
        language "C++"
        targetname "claw"

        ccgui_use_runtime_lib()
        kosmos_use_runtime_lib()
        putki_use_runtime_lib()
        
        putki_typedefs_runtime("src/types", true)

        files { "src/**.cpp" }
        files { "src/**.h" }

        excludes { "src/builder/**.*" }
        excludes { "src/putki/**.*" }

        includedirs { "src" }
        includedirs { "ext/squirrel-lang/include" }

        links {"squirrel-lang"}

    configuration {"windows"}
            excludes {"src/**_osx*"}
    
    configuration {"macosx"}
            excludes {"src/**_win32*"}
            files {"src/**.mm"}
            
    links {"AppKit.framework", "QuartzCore.framework", "OpenGL.framework"}
