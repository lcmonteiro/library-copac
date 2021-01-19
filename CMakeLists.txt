cmake_minimum_required (VERSION 3.13)

# project
project(copac)

# properties
set(CMAKE_CXX_STANDARD 17)

# target
add_library(copac INTERFACE)

# target configuration
target_include_directories(copac
INTERFACE
	./include
)

# test
enable_testing()
add_subdirectory(test)
