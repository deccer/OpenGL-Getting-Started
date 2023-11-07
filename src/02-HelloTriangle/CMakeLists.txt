add_custom_target(CopyData ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Data ${CMAKE_CURRENT_BINARY_DIR}/Data)

add_executable(02-HelloTriangle
    HelloTriangleApplication.cpp
    Main.cpp
)
add_dependencies(02-HelloTriangle CopyData)

target_include_directories(imgui PUBLIC include)
target_include_directories(spdlog PUBLIC include)

target_compile_options(02-HelloTriangle PUBLIC -Wall -Wextra -Werror)

target_link_libraries(02-HelloTriangle PRIVATE Shared glad glfw glm spdlog)