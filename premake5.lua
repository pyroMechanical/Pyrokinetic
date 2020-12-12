workspace "Pyrokinetic"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
	IncludeDir = {}
	IncludeDir["GLFW"] = "Pyrokinetic/ext/GLFW/include"
	
	include "Pyrokinetic/ext/GLFW"

project "Pyrokinetic"
	location "Pyrokinetic"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "pkpch.h"
	pchsource "src/pkpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/ext/spdlog/include",
		"%{IncludeDir.GLFW}"
	}
	
	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
		"PK_PLATFORM_WINDOWS",
		"PK_BUILD_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Editor")
		}

	filter "configurations:Debug"
		defines "PK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "PK_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "PK_DIST"
		symbols "On"


project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Pyrokinetic/src",
		"Pyrokinetic/ext/spdlog/include"
	}

	links
	{
		"Pyrokinetic"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
		"PK_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "PK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "PK_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "PK_DIST"
		symbols "On"