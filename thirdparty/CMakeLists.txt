add_library(loguru STATIC loguru/loguru.cpp)
target_include_directories(loguru PUBLIC loguru)
add_library(loguru::loguru ALIAS loguru)

add_subdirectory(glm)
target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_SWIZZLE)

add_subdirectory(json)

add_library(stb STATIC stb.cpp)
target_include_directories(stb PUBLIC . stb)

set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_BMP ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_TGA ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_JPG ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_GIF ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PSD ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PKM ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PVR ON CACHE BOOL "" FORCE)
add_subdirectory(raylib)

add_library(nanovg STATIC nanovg/src/nanovg.c nanovg/src/nanosvg.c nanovg/src/nanosvgrast.c)
target_link_libraries(nanovg PRIVATE stb)
target_include_directories(nanovg PUBLIC nanovg/src)
