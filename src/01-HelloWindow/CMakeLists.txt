add_executable(01-HelloWindow
    HelloWindowApplication.cpp
    Main.cpp
)

if (MSVC)
    target_compile_options(01-HelloWindow PRIVATE /W3 /WX)
else()
    target_compile_options(01-HelloWindow PRIVATE -Wall -Wextra -Werror)
endif()

target_include_directories(imgui PUBLIC include)
target_include_directories(spdlog PUBLIC include)

target_link_libraries(01-HelloWindow PRIVATE Shared glad glfw spdlog)