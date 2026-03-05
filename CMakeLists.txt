cmake_minimum_required(VERSION 3.27)
project(arkanoid_sfml)

set(CMAKE_CXX_STANDARD 17)

# Find SFML package
find_package(SFML 2.5 COMPONENTS system  graphics network audio REQUIRED)

add_executable(arkanoid_sfml main.cpp)

# Link SFML libraries
target_link_libraries(arkanoid_sfml sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
