cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(uComms)

# Enable testing for the entire project
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)