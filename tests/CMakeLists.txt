project(gluon_tests)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.0-preview4)

FetchContent_MakeAvailable(Catch2)

add_executable(${PROJECT_NAME} test_lexer.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE gluon_lang Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})
