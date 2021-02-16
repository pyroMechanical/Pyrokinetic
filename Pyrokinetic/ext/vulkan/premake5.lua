project "Vulkan"
	kind "StaticLib"
	language "C"
	staticruntime "on"
	
	targetdir ("bin/"..outputdir.."/%{prj.name}")
	objdir ("bin-int/"..outputdir.."/%{prj.name}")
	
		files
		{
			"include/vulkan/vulkan.h",
			"include/vulkan/vulkan.hpp"
		}
		
		includedirs
		{
			"include"
		}
	
		filter "system:windows"
			systemversion "latest"
			
		
		filter "configurations:Release"
			runtime "Release"
			optimize "on"
			
		filter "configurations:Debug"
			runtime "Debug"
			symbols "on"