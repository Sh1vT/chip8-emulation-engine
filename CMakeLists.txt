cmake_minimum_required(VERSION 3.15)
project(chip8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Tell CMake to use vcpkg toolchain file
# (This will be passed at configure time, not here.)

find_package(SDL2 CONFIG REQUIRED)

add_executable(chip8
    src/main.cpp
    src/Chip8.cpp
    src/SDL2Display.cpp
)


target_link_libraries(chip8 PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
