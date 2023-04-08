cmake_minimum_required(VERSION 3.14)
project(01-BasicTriangle-nonDSA)

target_include_directories(imgui PUBLIC include)
target_include_directories(spdlog PUBLIC include)

set(sourceFiles
    Main.cpp
)
add_executable(01-BasicTriangle-nonDSA ${sourceFiles} ${headerFiles})

target_compile_options(01-BasicTriangle-nonDSA PUBLIC -Wall -Wextra -Werror)

target_link_libraries(01-BasicTriangle-nonDSA PRIVATE glad glfw imgui glm cgltf stb_image spdlog debugbreak)
