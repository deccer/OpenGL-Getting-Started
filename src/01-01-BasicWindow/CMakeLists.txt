add_executable(01-01-BasicWindow
    Main.cpp
)

if (MSVC)
    target_compile_options(01-01-BasicWindow PRIVATE /W3 /WX)
else()
    target_compile_options(01-01-BasicWindow PRIVATE -Wall -Wextra -Werror)
endif()

target_include_directories(spdlog PUBLIC include)

target_link_libraries(01-01-BasicWindow PRIVATE glad glfw spdlog)