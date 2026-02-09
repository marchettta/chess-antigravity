cmake_minimum_required(VERSION 3.10)
project(Ajedrez)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# Include directories
include_directories(src/engine src/gui)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable
add_executable(ajedrez main.cpp ${SOURCES})

# Link SFML
target_link_libraries(ajedrez sfml-graphics sfml-window sfml-system sfml-audio)
