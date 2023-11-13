add_executable(01-02-BasicWindowAndTriangle
    Main.cpp
)

if (MSVC)
    target_compile_options(01-02-BasicWindowAndTriangle PRIVATE /W3 /WX)
else()
    target_compile_options(01-02-BasicWindowAndTriangle PRIVATE -Wall -Wextra -Werror)
endif()

target_include_directories(spdlog PUBLIC include)

target_link_libraries(01-02-BasicWindowAndTriangle PRIVATE glad glfw spdlog glm)