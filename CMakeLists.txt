cmake_minimum_required(VERSION 3.10)
project(FileMagik C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

# Packages
find_package(PkgConfig REQUIRED)
find_package(Curses REQUIRED)

# Sources
set(SOURCES
    main.c
    src/UI.c
)

# Executable
add_executable(termi-timer ${SOURCES})
target_link_libraries(termi-timer ${CURSES_LIBRARIES})
target_compile_options(termi-timer PRIVATE)
