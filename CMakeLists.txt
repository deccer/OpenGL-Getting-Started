cmake_minimum_required(VERSION 3.14)

project(OpenGLGettingStarted)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(src/Shared)
add_subdirectory(src/01-HelloWindow)
add_subdirectory(src/02-HelloTriangle)