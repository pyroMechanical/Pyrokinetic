cmake_minimum_required(VERSION 3.2)
project(Vulkan C)

add_library(Vulkan include/vulkan/vulkan.h include/vulkan/vulkan.hpp)
target_link_directories(Vulkan PUBLIC include/vulkan/)
