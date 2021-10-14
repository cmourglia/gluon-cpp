add_library(loguru STATIC loguru/loguru.cpp)
target_include_directories(loguru PUBLIC loguru)
add_library(loguru::loguru ALIAS loguru)

add_subdirectory(json)

set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_BMP ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_TGA ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_JPG ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_GIF ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PSD ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PKM ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PVR ON CACHE BOOL "" FORCE)
add_subdirectory(raylib)

set(BEARD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(BeardLibrary)

add_library(nanovg STATIC nanovg/src/nanosvg.c nanovg/src/nanosvgrast.c)
target_link_libraries(nanovg PRIVATE stb_truetype stb_image)
target_include_directories(nanovg PUBLIC nanovg/src)
