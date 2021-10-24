cmake_minimum_required(VERSION 3.17)
project(gluon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(thirdparty)


set(CORE_SRC
    src/Gluon/Core/Gluon_Types.cpp
    src/Gluon/Core/Gluon_Types.h
)

set(COMPILER_SRC
    src/Gluon/Compiler/Gluon_Parser.cpp
    src/Gluon/Compiler/Gluon_ShuntingYard.cpp
    src/Gluon/Compiler/Gluon_Tokenizer.cpp

    src/Gluon/Compiler/Gluon_Parser.h
    src/Gluon/Compiler/Gluon_ShuntingYard.h
    src/Gluon/Compiler/Gluon_Tokenizer.h
)

set(VM_SRC
    src/Gluon/VM/Gluon_AST.cpp
    src/Gluon/VM/Gluon_Cell.cpp
    src/Gluon/VM/Gluon_Function.cpp
    src/Gluon/VM/Gluon_Heap.cpp
    src/Gluon/VM/Gluon_Interpreter.cpp
    src/Gluon/VM/Gluon_Object.cpp
    src/Gluon/VM/Gluon_Value.cpp
    src/Gluon/VM/Gluon_Lexer.cpp
    src/Gluon/VM/Gluon_Parser.cpp

    src/Gluon/VM/Gluon_AST.h
    src/Gluon/VM/Gluon_Cell.h
    src/Gluon/VM/Gluon_Function.h
    src/Gluon/VM/Gluon_Heap.h
    src/Gluon/VM/Gluon_Interpreter.h
    src/Gluon/VM/Gluon_Object.h
    src/Gluon/VM/Gluon_Value.h
    src/Gluon/VM/Gluon_Lexer.h
    src/Gluon/VM/Gluon_Lexer.cpp
)

set(WIDGETS_SRC
    src/Gluon/Widgets/Gluon_Image.cpp
    src/Gluon/Widgets/Gluon_Rectangle.cpp
    src/Gluon/Widgets/Gluon_Text.cpp
    src/Gluon/Widgets/Gluon_Widget.cpp
    src/Gluon/Widgets/Gluon_Window.cpp

    src/Gluon/Widgets/Gluon_Image.h
    src/Gluon/Widgets/Gluon_Rectangle.h
    src/Gluon/Widgets/Gluon_Text.h
    src/Gluon/Widgets/Gluon_Widget.h
    src/Gluon/Widgets/Gluon_Window.h
)

set(APP_SRC
    src/Gluon/App/Gluon_App.cpp

    src/Gluon/App/Gluon_App.h
)

add_executable(gluon src/main.cpp
    ${CORE_SRC}
    ${COMPILER_SRC}
    ${VM_SRC}
    ${WIDGETS_SRC}
    ${APP_SRC}
)

target_link_libraries(${PROJECT_NAME} PRIVATE loguru::loguru glm::glm raylib nanovg stb_truetype BeardLibrary)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4> $<$<CXX_COMPILER_ID:MSVC>:/WX>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:GNUC>:-Wall> $<$<CXX_COMPILER_ID:GNU>:-Werror>)