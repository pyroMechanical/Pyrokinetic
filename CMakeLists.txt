cmake_minimum_required(VERSION 3.10...3.12)

Project(Pyrokinetic)

include(FindVulkan)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile options")
	add_compile_options(-Wall -Wextra -Wpedantic)
endif(CMAKE_COMPILER_IS_GNUCXX)


if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE pk_sources RELATIVE ${PROJECT_SOURCE_DIR} "Pyrokinetic/src/*.cpp" "Pyrokinetic/src/*.h")

foreach(_source IN ITEMS ${pk_sources})
	get_filename_component(source_path "${_source}" PATH)
	string(REPLACE "/" "\\" source_path_msvc "${source_path}")
	source_group("${source_path_msvc}" FILES "${_source}")
endforeach()

add_library(Pyrokinetic STATIC
	${pk_sources}
	"Pyrokinetic/ext/stb_image/stb_image.cpp"
	"Pyrokinetic/ext/vkbootstrap/src/VkBootstrap.cpp"
)



target_compile_definitions(Pyrokinetic PRIVATE PK_PLATFORM_WINDOWS)
target_precompile_headers(Pyrokinetic PRIVATE "Pyrokinetic/src/pkpch.h")

set(CMAKE_CXX_FLAGS_DEBUG -DPK_DEBUG)
set(CMAKE_CXX_FLAGS_RELEASE -DPK_RELEASE)
set(CMAKE_CXX_FLAGS_DIST -DPK_DIST)

target_include_directories(Pyrokinetic PUBLIC
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/src"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/spdlog/include"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/GLFW/include"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/Glad/include"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/vulkan"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/imgui"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/implot"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/glm"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/vkbootstrap/src"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/stb_image"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/vulkanmemoryallocator/src"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/entt/src"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/SPIRV-Tools/include"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/SPIRV-Cross/include"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/shaderc/libshaderc/include"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/yaml/include"
	"${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/crossguid/include"
)

#set(target_list "" CACHE INTERNAL "target_list")
#function(add_library name)
#	message("Adding library ${name}")
#	set(target_list ${target_list} ${name} CACHE INTERNAL "target_list")
#	_add_library(${name} ${ARGN})
#endfunction()

add_subdirectory(Pyrokinetic/ext/GLFW)
target_link_libraries(Pyrokinetic PUBLIC glfw)
target_compile_options(Pyrokinetic PRIVATE -DGLFW_INCLUDE_NONE)

add_subdirectory(Pyrokinetic/ext/Glad)
target_link_libraries(Pyrokinetic PUBLIC Glad)

add_subdirectory(Pyrokinetic/ext/imgui)
target_link_libraries(Pyrokinetic PUBLIC ImGui)

add_subdirectory(Pyrokinetic/ext/implot)
target_link_libraries(Pyrokinetic PUBLIC ImPlot)

add_subdirectory(Pyrokinetic/ext/entt)
target_link_libraries(Pyrokinetic PUBLIC EnTT)

set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
add_subdirectory(Pyrokinetic/ext/SPIRV-Tools)
target_link_libraries(Pyrokinetic PUBLIC SPIRV-Tools)

set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(Pyrokinetic/ext/glslang)
target_link_libraries(Pyrokinetic PUBLIC glslang)

set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
add_subdirectory(Pyrokinetic/ext/shaderc)
target_link_libraries(Pyrokinetic PUBLIC shaderc)

set(SPIRV_CROSS_SHARED ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(Pyrokinetic/ext/SPIRV-Cross)
target_link_libraries(Pyrokinetic PUBLIC spirv-cross-c)
			
add_subdirectory(Pyrokinetic/ext/yaml)
target_link_libraries(Pyrokinetic PUBLIC yaml-cpp)

set(CROSSGUID_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(Pyrokinetic/ext/crossguid)
target_link_libraries(Pyrokinetic PUBLIC crossguid)

target_link_libraries(Pyrokinetic PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(Pyrokinetic PUBLIC ${Vulkan_INCLUDE_DIRS})
target_compile_options(Pyrokinetic PUBLIC ${Vulkan_CFLAGS_OTHER})

#set_target_properties(${target_list} PROPERTIES FOLDER "Dependencies")


#add_subdirectory(ext/vulkanmemoryallocator)
#target_link_libraries(Pyrokinetic PUBLIC VMA)
#target_compile_options(Pyrokinetic PUBLIC -DVMA_IMPLEMENTATION)

file(GLOB_RECURSE editor_sources RELATIVE ${PROJECT_SOURCE_DIR} "Editor/src/*.cpp" "Editor/src/*.h" "Editor/src/*.rc")
add_executable(Editor ${editor_sources})

foreach(_source IN ITEMS ${editor_sources})
	get_filename_component(source_path "${_source}" PATH)
	string(REPLACE "/" "\\" source_path_msvc "${source_path}")
	source_group("${source_path_msvc}" FILES "${_source}")
endforeach()

target_include_directories(Editor PUBLIC
 "${PROJECT_SOURCE_DIR}/Pyrokinetic/src"
 "${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/spdlog/include"
 "${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/glm"
 "${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/vulkan"
 "${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/entt/src"
 "${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/SPIRV-Tools/include"
 "${PROJECT_SOURCE_DIR}/Pyrokinetic/ext/SPIRV-Cross/include"
 )

target_link_libraries(Editor PRIVATE Pyrokinetic)