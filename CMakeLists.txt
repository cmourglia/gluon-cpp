cmake_minimum_required(VERSION 3.17)
project(gluon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(thirdparty)

add_executable(gluon src/main.cpp
    src/gluon_app.cpp
    src/types.cpp
    src/utils.cpp

    src/compiler/tokenizer.cpp
    src/compiler/parser.cpp
    src/compiler/shuntingyard.cpp

    src/widgets/widget.cpp
    src/widgets/window.cpp
    src/widgets/rectangle.cpp
    src/widgets/image.cpp
    src/widgets/text.cpp

    src/gluon_app.h
    src/types.h
    src/utils.h
    src/defines.h

    src/compiler/tokenizer.h
    src/compiler/parser.h
    src/compiler/shuntingyard.h

    src/widgets/widget.h
    src/widgets/window.h
    src/widgets/rectangle.h
    src/widgets/image.h
    src/widgets/text.h
    src/widgets/hashes.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE loguru::loguru glm::glm raylib stb nanovg)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_include_directories(${PROJECT_NAME} PRIVATE src)