cmake_minimum_required(VERSION 3.17)
project(gluon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(thirdparty)

add_executable(gluon src/main.cpp
        src/gluon_app.cpp src/types.cpp src/utils.cpp src/compiler/tokenizer.cpp src/compiler/parser.cpp src/compiler/shuntingyard.cpp src/compiler/widget.cpp
        src/gluon_app.h src/types.h src/utils.h src/compiler/tokenizer.h src/compiler/parser.h src/compiler/shuntingyard.h src/compiler/widget.h
        src/defines.h)
target_link_libraries(gluon PRIVATE loguru::loguru glm::glm raylib)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_include_directories(${PROJECT_NAME} PRIVATE src)