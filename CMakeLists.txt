cmake_minimum_required(VERSION 3.18)
project(gluon)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(thirdparty)

include(thirdparty/VulkanUtils/CMake/ShaderUtils.cmake)

set(SHADER_SRC ${CMAKE_SOURCE_DIR}/shaders/src)
set(SHADER_BIN ${CMAKE_SOURCE_DIR}/shaders/bin)

set(IN_SHADERS triangle.vert.glsl triangle.geom.glsl triangle.frag.glsl)
set(OUT_SHADERS )

foreach(SHADER ${IN_SHADERS})
    compile_shader(${SHADER} ${SHADER_SRC} ${SHADER_BIN} OUT_SHADER)
    list(APPEND OUT_SHADERS ${OUT_SHADER})
endforeach()

add_executable(gluon src/main.cpp
    src/gluon_app.cpp src/types.cpp src/utils.cpp src/compiler/tokenizer.cpp src/compiler/parser.cpp
    src/gluon_app.h src/types.h src/utils.h src/compiler/tokenizer.h src/compiler/parser.h
    src/defines.h
    ${OUT_SHADERS})
target_link_libraries(gluon PRIVATE loguru::loguru glfw::glfw glm::glm bgfx)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_include_directories(${PROJECT_NAME} PRIVATE src)