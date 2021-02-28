# add_subdirectory(glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
add_library(glfw::glfw ALIAS glfw)

add_library(loguru STATIC loguru/loguru.cpp)
target_include_directories(loguru PUBLIC loguru)
add_library(loguru::loguru ALIAS loguru)

find_package(Vulkan REQUIRED)
add_library(volk STATIC volk/volk.c)
target_include_directories(volk PUBLIC volk ${Vulkan_INCLUDE_DIRS})
add_library(volk::volk ALIAS volk)

add_library(hmm HandmadeMath/HandmadeMath.cpp)
target_include_directories(hmm PUBLIC HandmadeMath)
add_library(hmm::hmm ALIAS hmm)

add_subdirectory(glm)
target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_SWIZZLE)

add_subdirectory(json)

# add_library(vma vma.cpp)
# target_include_directories(vma PUBLIC VulkanMemoryAllocator/src)
# target_compile_definitions(vma PUBLIC VMA_STATIC_VULKAN_FUNCTIONS=0 VMA_DYNAMIC_VULKAN_FUNCTIONS=1 VMA_MEMORY_BUDGET=0)
# target_link_libraries(vma PUBLIC volk::volk)
# add_library(vma::vma ALIAS vma)