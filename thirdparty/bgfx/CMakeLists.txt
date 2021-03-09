set(BX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bx)
set(BX_SRC ${BX_DIR}/src)
set(BX_3RD ${BX_DIR}/3rdparty)

set(BIMG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bimg)
set(BIMG_SRC ${BIMG_DIR}/src)
set(BIMG_3RD ${BIMG_DIR}/3rdparty)

set(BGFX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bgfx)
set(BGFX_SRC ${BGFX_DIR}/src)
set(BGFX_3RD ${BGFX_DIR}/3rdparty)

add_library(bx STATIC ${BX_SRC}/amalgamated.cpp)
target_include_directories(bx PUBLIC ${BX_DIR}/include ${BX_3RD} $<$<CXX_COMPILER_ID:MSVC>:${BX_DIR}/include/compat/msvc>)
target_compile_definitions(bx PUBLIC $<$<CXX_COMPILER_ID:MSVC>:__STDC_LIMIT_MACROS __STDC_FORMAT_MACROS __STDC_CONSTANT_MACROS>)

set(ASTC_DIR ${BIMG_3RD}/astc-codec)
file(GLOB ASTC_SRCS ${ASTC_DIR}/src/decoder/*.cc)
add_library(astc-codec STATIC ${ASTC_SRCS})
target_include_directories(astc-codec PUBLIC ${ASTC_DIR}/include ${ASTC_DIR})

file(GLOB BIMG_SRCS ${BIMG_SRC}/*.cpp)
add_library(bimg STATIC ${BIMG_SRCS})
target_include_directories(bimg PUBLIC bimg/include ${BIMG_3RD} ${BIMG_3RD}/iqa/include)
target_link_libraries(bimg PUBLIC bx astc-codec)

find_package(OpenGL REQUIRED)

add_library(bgfx STATIC ${BGFX_SRC}/amalgamated.cpp)
target_include_directories(bgfx PUBLIC ${BGFX_DIR}/include ${BGFX_3RD} ${BGFX_3RD}/khronos $<$<CXX_COMPILER_ID:MSVC>:${BGFX_3RD}/dxsdk/include>)
target_link_libraries(bgfx PUBLIC bimg $<$<CXX_COMPILER_ID:GCC>:X11> ${OPENGL_LIBRARIES})