cmake_minimum_required(VERSION 3.14)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif(CCACHE_FOUND)

project(OpenGLGettingStarted)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)
add_subdirectory(src/Shared)
add_subdirectory(src/01-HelloWindow)
add_subdirectory(src/02-HelloTriangleBasic)
add_subdirectory(src/03-HelloTriangle)