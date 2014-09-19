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
	
	dofile "external/putki/putkilib-premake.lua"

	-- Depends on putki
	dofile "external/ccg-ui/ccg-ui-libs.lua"

	project "claw-putki-lib"

		language "C++"
		targetname "claw-putki-lib"

		kind "StaticLib"
		
		files { "src/types/**.typedef" }
		files { "_gen/*putki-master.cpp", "_gen/inki/**.h", "_gen/data-dll/**.h" }

		includedirs { "src", "_gen" }
		includedirs ( PUTKI_LIB_INCLUDES )
		includedirs ( CCGUI_LIB_INCLUDES )

		links (CCGUI_LIB_LINKS)
		links (PUTKI_LIB_LINKS)


	project "claw-databuilder"

		kind "ConsoleApp"
		language "C++"
		targetname "claw-databuilder"

		files { "src/putki/builder-main.cpp" }
		files { "src/builder/**.*" }

		includedirs { "src", "_gen" }
		includedirs ( PUTKI_LIB_INCLUDES )
		includedirs ( CCGUI_LIB_INCLUDES )
		
		links { "claw-putki-lib" }
		links ( CCGUI_LIB_LINKS )
		links ( PUTKI_LIB_LINKS )

	project "claw-datatool"

		kind "ConsoleApp"
		language "C++"
		targetname "claw-datatool"

		files { "src/putki/tool-main.cpp" }

		includedirs { "src", "_gen" }
		includedirs ( PUTKI_LIB_INCLUDES )
		includedirs ( CCGUI_LIB_INCLUDES )
		
		links { "claw-putki-lib" }
		links ( CCGUI_LIB_LINKS )
		links ( PUTKI_LIB_LINKS )


	project "squirrel-lang"
		kind "StaticLib"
		language "C++"
		targetname "squirrel"
		
		files { "external/squirrel-lang/squirrel/*.cpp" }
		includedirs { "external/squirrel-lang/squirrel", "external/squirrel-lang/include" }
		

if os.get() ~= "linux" and os.get() ~= "bsd" then

	project "claw-data-dll"

		kind "SharedLib"
		language "C++"
		targetname "claw-data-dll"

		files { "src/putki/dll-main.cpp" }
		files { "src/builder/**.*" }		

		includedirs { "src", "_gen" }
		includedirs ( PUTKI_LIB_INCLUDES )
		includedirs ( CCGUI_LIB_INCLUDES )
	
		links { "claw-putki-lib"}
		links ( CCGUI_LIB_LINKS )
		links ( PUTKI_LIB_LINKS )


 	project "claw-runtime"

            kind "ConsoleApp"
            language "C++"
            targetname "claw"

            files { "_gen/outki/**.cpp", "_gen/outki/**.h" }
            files { "src/**.cpp" }
            files { "src/**.h" }
            files { "src/**.typedef" }

            excludes { "src/builder/**.*" }
            excludes { "src/putki/**.*" }

	includedirs { "src", "_gen" }
	includedirs ( PUTKI_RT_INCLUDES )
	includedirs ( CCGUI_RT_INCLUDES )
	includedirs { "external/squirrel-lang/include" }

	links {"ccg-runtime"}
	links {"putki-runtime-lib"}
	links {"squirrel-lang"}

            configuration {"windows"}
                    excludes {"src/**_osx*"}
            
            configuration {"macosx"}
                    excludes {"src/**_win32*"}
                    files {"src/**.mm"}
                    links {"AppKit.framework", "QuartzCore.framework", "OpenGL.framework"}

end
