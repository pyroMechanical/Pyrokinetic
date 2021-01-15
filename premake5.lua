workspace "Pyrokinetic"
	architecture "x64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
	IncludeDir = {}
	IncludeDir["GLFW"] =  "Pyrokinetic/ext/GLFW/include"
	IncludeDir["Glad"] =  "Pyrokinetic/ext/Glad/include"
	IncludeDir["ImGui"] = "Pyrokinetic/ext/imgui"
	IncludeDir["ImPlot"] = "Pyrokinetic/ext/implot"
	IncludeDir["glm"] = "Pyrokinetic/ext/glm"
	IncludeDir["stb_image"] = "Pyrokinetic/ext/stb_image"
	
	
	group "Dependencies"
		include "Pyrokinetic/ext/GLFW"
		include "Pyrokinetic/ext/Glad"
		include "Pyrokinetic/ext/imgui"
		include "Pyrokinetic/ext/implot"
	group ""

project "Pyrokinetic"
	location "Pyrokinetic"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "pkpch.h"
	pchsource "Pyrokinetic/src/pkpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/ext/stb_image/**.cpp",
		"%{prj.name}/ext/stb_image/**.h",
		"%{prj.name}/ext/glm/glm/**.hpp",
		"%{prj.name}/ext/glm/glm/**.inl"
	}
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/ext/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImPlot}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}
	
	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"ImPlot",
		"opengl32.lib",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "PK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "PK_DIST"
		runtime "Release"
		symbols "on"


project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
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
		"Pyrokinetic/ext/spdlog/include",
		"Pyrokinetic/ext/imgui",
		"Pyrokinetic/ext/implot",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Pyrokinetic"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "PK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "PK_DIST"
		runtime "Release"
		symbols "on"