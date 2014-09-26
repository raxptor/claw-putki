solution "Claw"

	if os.get() ~= "linux" and os.get() ~= "bsd" then
		platforms { "x32" }
	end
		
	configurations {"Release", "Debug"}

	location "build"
	targetdir "build"
	flags { "Symbols" }
	defines {"_CRT_SECURE_NO_WARNINGS"}

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
	
	------------------------------------
	-- Putki must always come first   --
	------------------------------------

	dofile "ext/putki/libs.lua"
	dofile "ext/kosmos/libs.lua"
	dofile "ext/ccg-ui/libs.lua"

	project "claw-putki-lib"
		language "C++"
		targetname "claw-putki-lib"
		kind "StaticLib"
		
		putki_use_builder_lib()
		kosmos_use_builder_lib()
		ccgui_use_builder_lib()
		
		putki_typedefs_builder("src/types", true)

	project "claw-databuilder"

		kind "ConsoleApp"
		language "C++"
		targetname "claw-databuilder"

		files { "src/putki/builder-main.cpp" }
		files { "src/builder/**.cpp" }
		links { "claw-putki-lib" }
		includedirs { "src" }
		
		putki_use_builder_lib()
		kosmos_use_builder_lib()
		ccgui_use_builder_lib()
		
		putki_typedefs_builder("src/types", false)

	project "claw-datatool"

		kind "ConsoleApp"
		language "C++"
		targetname "claw-datatool"

		files { "src/putki/tool-main.cpp" }

		putki_use_builder_lib()
		kosmos_use_builder_lib()
		ccgui_use_builder_lib()

		includedirs { "src" }
		links { "claw-putki-lib" }

	project "squirrel-lang"
		kind "StaticLib"
		language "C++"
		targetname "squirrel"
		platforms { "x64" }
		
		files { "ext/squirrel-lang/squirrel/*.cpp" }
		includedirs { "ext/squirrel-lang/squirrel", "ext/squirrel-lang/include" }
		

if os.get() ~= "linux" and os.get() ~= "bsd" then

	project "claw-data-dll"

		kind "SharedLib"
		language "C++"
		targetname "claw-data-dll"

		files { "src/putki/dll-main.cpp" }
		files { "src/builder/**.*" }
		links { "claw-putki-lib"}
		includedirs { "src" }
		
		putki_typedefs_builder("src/types", false)
		putki_use_builder_lib()
		kosmos_use_builder_lib()
		ccgui_use_builder_lib()

 	project "claw-runtime"
 		platforms { "x64" }

                kind "ConsoleApp"
                language "C++"
                targetname "claw"
                
                putki_use_runtime_lib()
                kosmos_use_runtime_lib()
		ccgui_use_runtime_lib()
		
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

end
