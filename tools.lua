solution "Tools"

	if os.get() ~= "linux" and os.get() ~= "bsd" then
		platforms { "x32" }
	end
		
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
		links { "claw-putki-lib" }

		includedirs { "src" }
		
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
