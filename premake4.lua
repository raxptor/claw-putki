solution "Claw"

	platforms { "x32" }

	configurations {"Release", "Debug"}

	location "build"
	targetdir "build"
	flags { "Symbols" }
	defines {"_CRT_SECURE_NO_WARNINGS"}

	if os.get() == "windows" then
		flags {"StaticRuntime"}
	end

	configuration "Debug"
		defines {"DEBUG"}

	dofile "external/putki/ccg-ui/ccg-ui-libs.lua"

	project "claw-putki-lib"

		if os.get() == "windows" then
			kind "StaticLib"
		else
			kind "SharedLib"
		end

		language "C++"

		targetname "claw-putki-lib"
		
		files { "src/types/**.typedef" }
		files {  "_gen/inki/**.h", "_gen/data-dll/**.h" }
		files { "_gen/*-putki-master.cpp",  }

		includedirs { "src", "src/builder", "src/data-dll" }
		includedirs { "_gen" }

		includedirs { "external/putki/ccg-ui/src", "external/putki/ccg-ui/src/builder", "external/putki/ccg-ui-/src/data-dll" }
		includedirs { "external/putki/ccg-ui/_gen" }

		includedirs { "external/putki/src" }
		includedirs { "external/putki/src/data-dll" }
		
		links {"ccg-ui-putki-lib"}

	project "claw-databuilder"

		kind "ConsoleApp"
		language "C++"
		targetname "claw-databuilder"

		includedirs { "_gen" }
		includedirs { "external/putki/ccg-ui/_gen" }
		includedirs { "external/putki/src" }

		files { "src/putki/builder-main.cpp" }
		files { "src/builder/**.*" }

		links { "claw-putki-lib"}
		links { "ccg-ui-databuilder"}
		links { "ccg-ui-putki-lib"}
		links { "putki-lib"}

	project "claw-data-dll"

		kind "SharedLib"
		language "C++"
		targetname "claw-data-dll"

		files { "src/putki/dll-main.cpp" }

		includedirs { "external/putki/ccg-ui/_gen" }
		includedirs { "_gen" }

		includedirs { "external/putki/src" }
		includedirs { "external/putki/src/data-dll" }

		links { "claw-putki-lib"}
		links { "ccg-ui-databuilder"}
		links { "ccg-ui-putki-lib"}
		links { "putki-lib"}

	project "claw-putked-typelib"

		kind "SharedLib"
		language "C#"
		targetname "claw-putked-typelib"
		files {"_gen/inki_csharp/**.cs"}
		links {"ccg-ui-putked-typelib"}
		links {"putked-lib"}		

        project "claw-runtime"
                kind "ConsoleApp"
                language "C++"

                targetname "claw"
                files { "external/putki/src/cpp-runtime/**.cpp", "external/putki/src/cpp-runtime/**.h" }
                files { "_gen/outki/**.cpp", "_gen/outki/**.h" }
                files { "src/**.cpp" }
                files { "src/**.h" }
                files { "src/**.typedef" }

                excludes { "src/builder/**.*" }
                excludes { "src/putki/**.*" }

                includedirs { "external/putki/src/cpp-runtime/", "_gen", "src" }
		includedirs { "external/putki/ccg-ui/_gen" } 
                includedirs { "external/putki/ccg-ui/src/cpp-runtime" }

		links {"ccg-runtime"}

                configuration {"windows"}
                        excludes {"src/**_osx*"}
                
                configuration {"macosx"}
                        excludes {"src/**_win32*"}
                        files {"src/**.mm"}
                        links {"AppKit.framework", "QuartzCore.framework", "OpenGL.framework"}

