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
	IncludeDir["Glad"] = "Pyrokinetic/ext/Glad/include"
	IncludeDir["ImGui"] = "Pyrokinetic/ext/imgui"
	
	include "Pyrokinetic/ext/GLFW"
	include "Pyrokinetic/ext/Glad"
	include "Pyrokinetic/ext/imgui"

project "Pyrokinetic"
	location "Pyrokinetic"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "pkpch.h"
	pchsource "Pyrokinetic/src/pkpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/ext/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}
	
	links
	{
		"GLFW",
		"Glad",
		"ImGui",
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
		"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Editor")
		}

	filter "configurations:Debug"
		defines "PK_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "PK_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "PK_DIST"
		buildoptions "/MD"
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
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "PK_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "PK_DIST"
		buildoptions "/MD"
		symbols "On"