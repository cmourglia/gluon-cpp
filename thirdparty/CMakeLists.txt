set(BEARD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(BeardLibrary)

add_subdirectory(SDL)
add_subdirectory(SDL_image)

add_library(nanovg STATIC nanovg/src/nanosvg.c nanovg/src/nanosvgrast.c)
target_link_libraries(nanovg PRIVATE stb_truetype stb_image)
target_include_directories(nanovg PUBLIC nanovg/src)
