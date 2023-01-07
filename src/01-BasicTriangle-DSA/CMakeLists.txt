cmake_minimum_required(VERSION 3.14)
project(01-BasicTriangle-DSA)

target_include_directories(imgui PUBLIC include)
target_include_directories(spdlog PUBLIC include)

set(sourceFiles
    Main.cpp
)
add_executable(01-BasicTriangle-DSA ${sourceFiles} ${headerFiles})

target_link_libraries(01-BasicTriangle-DSA PRIVATE glad glfw imgui glm cgltf stb_image spdlog debugbreak)
