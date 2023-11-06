add_executable(02-HelloTriangle
    HelloTriangleApplication.cpp
    Main.cpp
)

target_include_directories(imgui PUBLIC include)
target_include_directories(spdlog PUBLIC include)

target_compile_options(02-HelloTriangle PUBLIC -Wall -Wextra -Werror)

target_link_libraries(02-HelloTriangle PRIVATE Shared glad glfw glm)