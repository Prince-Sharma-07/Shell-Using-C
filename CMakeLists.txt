# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(SimpleShell VERSION 1.0)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Add executable target
add_executable(simpleShell simpleShell.c)

