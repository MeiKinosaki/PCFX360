cmake_minimum_required(VERSION 3.16)
project(PCFX360 VERSION 0.9 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(pcfx_core src/core/memory.cpp src/core/memory_bus.cpp src/core/v810.cpp)
target_include_directories(pcfx_core PUBLIC include)
target_compile_options(pcfx_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wconversion>
)

include(CTest)
if(BUILD_TESTING)
    add_executable(pcfx_tests tests/test_core.cpp)
    target_link_libraries(pcfx_tests PRIVATE pcfx_core)
    add_test(NAME pcfx_core_tests COMMAND pcfx_tests)
endif()
