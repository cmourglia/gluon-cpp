add_library(bx STATIC bx/src/amalgamated.cpp)
target_include_directories(bx PUBLIC bx/include bx/3rdparty)

set(ASTC_DIR bimg/3rdparty/astc-codec)
file(GLOB ASTC_SRC ${ASTC_DIR}/src/decoder/*.cc)
add_library(astc-codec STATIC ${ASTC_SRC})
target_include_directories(astc-codec PUBLIC ${ASTC_DIR}/include ${ASTC_DIR})

add_library(bimg STATIC bimg/src/image_cubemap_filter.cpp bimg/src/image_decode.cpp bimg/src/image_encode.cpp bimg/src/image_gnf.cpp bimg/src/image.cpp)
target_include_directories(bimg PUBLIC bimg/include bimg/3rdparty bimg/3rdparty/iqa/include)
target_link_libraries(bimg PUBLIC bx astc-codec)

find_package(OpenGL REQUIRED)

add_library(bgfx STATIC bgfx/src/amalgamated.cpp)
target_include_directories(bgfx PUBLIC bgfx/include bgfx/3rdparty bgfx/3rdparty/khronos)
target_link_libraries(bgfx PUBLIC bimg X11 ${OPENGL_LIBRARIES})