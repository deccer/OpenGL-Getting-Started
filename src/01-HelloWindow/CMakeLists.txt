add_executable(01-HelloWindow
    Main.cpp
)

target_include_directories(imgui PUBLIC include)
target_include_directories(spdlog PUBLIC include)

target_compile_options(01-HelloWindow PUBLIC -Wall -Wextra -Werror)

target_link_libraries(01-HelloWindow PRIVATE Shared glad glfw)