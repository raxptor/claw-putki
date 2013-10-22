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

	dofile "../ccg-ui/ccg-ui-libs.lua"

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

		includedirs { "../ccg-ui/src", "../ccg-ui/src/builder", "../ccg-ui-/src/data-dll" }
		includedirs { "../ccg-ui/_gen" }

		includedirs { "../src" }
		includedirs { "../src/data-dll" }
		
		links {"ccg-ui-putki-lib"}

	project "claw-databuilder"

		kind "ConsoleApp"
		language "C++"
		targetname "claw-databuilder"

		includedirs { "_gen" }
		includedirs { "../ccg-ui/_gen" }
		includedirs { "../src" }

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

		includedirs { "../ccg-ui/_gen" }
		includedirs { "_gen" }

		includedirs { "../src" }
		includedirs { "../src/data-dll" }

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
		
 if os.get() == "windows" then

	project "claw-csharp"

		kind "WindowedApp"
		language "C#"
		targetname "claw-csharp"

		files { "src/viewer/**.cs" }
		files { "_gen/outki_csharp/**.cs" }

		links { "ccg-ui-csharp" }
		links { "PresentationFramework", "WindowsBase", "PresentationCore", "System.Xaml", "System" }
	
	end
