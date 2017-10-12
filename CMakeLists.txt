cmake_minimum_required(VERSION 2.6)

project(brainfuck)
add_executable(brainfuck src/brainfuck.c src/main.c)

project(brainfuck_sdl)
find_package(SDL2)
add_executable(brainfuck_sdl src/brainfuck.c src/sdl.c)
target_link_libraries(brainfuck_sdl ${SDL2_LIBRARIES})

project(brainfuck_tests)
include_directories(src)
add_executable(brainfuck_tests src/brainfuck.c tests/tests.c)
