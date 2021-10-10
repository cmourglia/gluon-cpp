cmake_minimum_required(VERSION 3.17)
project(gluon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(thirdparty)


set(CORE_SRC
    src/Gluon/Core/Timer.cpp
    src/Gluon/Core/Types.cpp
    src/Gluon/Core/Utils.cpp

    src/Gluon/Core/Defines.h
    src/Gluon/Core/Timer.h
    src/Gluon/Core/Types.h
    src/Gluon/Core/Utils.h

    src/Gluon/Core/Containers/HashMap.h
    src/Gluon/Core/Containers/DynArray.h
)

set(COMPILER_SRC
    src/Gluon/Compiler/Parser.cpp
    src/Gluon/Compiler/ShuntingYard.cpp
    src/Gluon/Compiler/Tokenizer.cpp

    src/Gluon/Compiler/Parser.h
    src/Gluon/Compiler/ShuntingYard.h
    src/Gluon/Compiler/Tokenizer.h
)

set(VM_SRC
    src/Gluon/VM/AST.cpp
    src/Gluon/VM/Cell.cpp
    src/Gluon/VM/Function.cpp
    src/Gluon/VM/Heap.cpp
    src/Gluon/VM/Interpreter.cpp
    src/Gluon/VM/Object.cpp
    src/Gluon/VM/Value.cpp

    src/Gluon/VM/AST.h
    src/Gluon/VM/Cell.h
    src/Gluon/VM/Function.h
    src/Gluon/VM/Heap.h
    src/Gluon/VM/Interpreter.h
    src/Gluon/VM/Object.h
    src/Gluon/VM/Value.h
)

set(WIDGETS_SRC
    src/Gluon/Widgets/Image.cpp
    src/Gluon/Widgets/Rectangle.cpp
    src/Gluon/Widgets/Text.cpp
    src/Gluon/Widgets/Widget.cpp
    src/Gluon/Widgets/Window.cpp

    src/Gluon/Widgets/Image.h
    src/Gluon/Widgets/Rectangle.h
    src/Gluon/Widgets/Text.h
    src/Gluon/Widgets/Widget.h
    src/Gluon/Widgets/Window.h
)

set(APP_SRC
    src/Gluon/App/GluonApp.cpp

    src/Gluon/App/GluonApp.h
)

add_executable(gluon src/main.cpp
    ${CORE_SRC}
    ${COMPILER_SRC}
    ${VM_SRC}
    ${WIDGETS_SRC}
    ${APP_SRC}
)

target_link_libraries(${PROJECT_NAME} PRIVATE loguru::loguru glm::glm raylib stb nanovg)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4> $<$<CXX_COMPILER_ID:MSVC>:/WX>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:GNUC>:-Wall> $<$<CXX_COMPILER_ID:GNU>:-Werror>)