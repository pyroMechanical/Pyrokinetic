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
)

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

add_subdirectory(Pyrokinetic/ext/SPIRV-Tools)
target_link_libraries(Pyrokinetic PUBLIC SPIRV-Tools)

add_subdirectory(Pyrokinetic/ext/glslang)
target_link_libraries(Pyrokinetic PUBLIC glslang)

add_subdirectory(Pyrokinetic/ext/shaderc)
target_link_libraries(Pyrokinetic PUBLIC shaderc)

add_subdirectory(Pyrokinetic/ext/SPIRV-Cross)
option(SPIRV_CROSS_SHARED ON)
target_link_libraries(Pyrokinetic PUBLIC
			spirv-cross-c)

target_link_libraries(Pyrokinetic PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(Pyrokinetic PUBLIC ${Vulkan_INCLUDE_DIRS})
target_compile_options(Pyrokinetic PUBLIC ${Vulkan_CFLAGS_OTHER})

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