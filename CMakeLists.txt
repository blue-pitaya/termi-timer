cmake_minimum_required(VERSION 3.10)
project(FileMagik C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

# Packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
find_package(Curses REQUIRED)

# Lib config
include_directories(${GLIB2_INCLUDE_DIRS})
link_directories(${GLIB2_LIBRARY_DIRS})

# Sources
set(SOURCES
    main.c
    src/UI.c
)

# Executable
add_executable(main ${SOURCES})
target_link_libraries(main ${GLIB2_LIBRARIES} ${CURSES_LIBRARIES})
target_compile_options(main PRIVATE ${GLIB2_CFLAGS_OTHER})
