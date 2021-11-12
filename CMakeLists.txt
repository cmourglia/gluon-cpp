cmake_minimum_required(VERSION 3.17)
project(gluon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(thirdparty)


set(CORE_SRC
        src/gluon/core/types.cpp
        src/gluon/core/types.h
)

set(COMPILER_SRC
        src/gluon/compiler/parser.cpp
        src/gluon/compiler/shunting_yard.cpp
        src/gluon/compiler/tokenizer.cpp

        src/gluon/compiler/parser.h
        src/gluon/compiler/shunting_yard.h
        src/gluon/compiler/tokenizer.h
)

set(LANGUAGE_SRC
        src/gluon/lang/ast.cpp
        src/gluon/lang/ast_printer.cpp
        src/gluon/lang/cell.cpp
        src/gluon/lang/function.cpp
        src/gluon/lang/heap.cpp
        src/gluon/lang/interpreter.cpp
        src/gluon/lang/object.cpp
        src/gluon/lang/value.cpp
        src/gluon/lang/lexer.cpp
        src/gluon/lang/parser.cpp

        src/gluon/lang/ast.h
        src/gluon/lang/ast_printer.h
        src/gluon/lang/grammar.h
        src/gluon/lang/token.h
        src/gluon/lang/cell.h
        src/gluon/lang/function.h
        src/gluon/lang/heap.h
        src/gluon/lang/interpreter.h
        src/gluon/lang/object.h
        src/gluon/lang/value.h
        src/gluon/lang/lexer.h
        src/gluon/lang/lexer.cpp
)

set(WIDGETS_SRC
        src/gluon/widgets/image.cpp
        src/gluon/widgets/rectangle.cpp
        src/gluon/widgets/text.cpp
        src/gluon/widgets/widget.cpp
        src/gluon/widgets/window.cpp

        src/gluon/widgets/image.h
        src/gluon/widgets/rectangle.h
        src/gluon/widgets/text.h
        src/gluon/widgets/widget.h
        src/gluon/widgets/window.h
)

set(APP_SRC
        src/gluon/app/app.cpp

        src/gluon/app/app.h
)

add_executable(gluon src/main.cpp
    ${CORE_SRC}
    ${COMPILER_SRC}
    ${LANGUAGE_SRC}
    ${WIDGETS_SRC}
    ${APP_SRC}
)

target_link_libraries(${PROJECT_NAME} PRIVATE loguru::loguru glm::glm raylib nanovg stb_truetype beardLibrary)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4> $<$<CXX_COMPILER_ID:MSVC>:/WX>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:GNUC>:-Wall> $<$<CXX_COMPILER_ID:GNU>:-Werror>)