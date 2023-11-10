add_executable(02-HelloTriangleBasic
    HelloTriangleBasicApplication.cpp
    Main.cpp
)

if (MSVC)
    target_compile_options(01-HelloWindow PRIVATE /W3 /WX)
else()
    target_compile_options(01-HelloWindow PRIVATE -Wall -Wextra -Werror)
endif()

target_include_directories(imgui PUBLIC include)
target_include_directories(spdlog PUBLIC include)

target_link_libraries(02-HelloTriangleBasic PRIVATE Shared glad glfw glm spdlog)

add_custom_command(
    TARGET 02-HelloTriangleBasic
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Data ${CMAKE_CURRENT_BINARY_DIR}/Data
)