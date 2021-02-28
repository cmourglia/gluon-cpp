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
    src/gluon_app.cpp src/resources.cpp src/shader.cpp src/swapchain.cpp src/vkhelpers.cpp src/types.cpp src/compiler/tokenizer.cpp src/compiler/parser.cpp
    src/gluon_app.h src/resources.h src/shader.h src/swapchain.h src/vkhelpers.h src/types.h src/compiler/tokenizer.h src/compiler/parser.h
    src/vkcommon.h src/defines.h
    ${OUT_SHADERS})
target_link_libraries(gluon PRIVATE volk::volk loguru::loguru glfw::glfw glm::glm nlohmann_json::nlohmann_json)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_include_directories(${PROJECT_NAME} PRIVATE src)