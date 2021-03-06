# add_subdirectory(glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
add_library(glfw::glfw ALIAS glfw)

add_library(loguru STATIC loguru/loguru.cpp)
target_include_directories(loguru PUBLIC loguru)
add_library(loguru::loguru ALIAS loguru)

add_subdirectory(glm)
target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_SWIZZLE)

add_subdirectory(json)

add_subdirectory(bgfx)

# add_library(vma vma.cpp)
# target_include_directories(vma PUBLIC VulkanMemoryAllocator/src)
# target_compile_definitions(vma PUBLIC VMA_STATIC_VULKAN_FUNCTIONS=0 VMA_DYNAMIC_VULKAN_FUNCTIONS=1 VMA_MEMORY_BUDGET=0)
# target_link_libraries(vma PUBLIC volk::volk)
# add_library(vma::vma ALIAS vma)