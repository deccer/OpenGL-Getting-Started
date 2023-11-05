add_library(Shared
    Application.cpp
)

target_link_libraries(Shared PRIVATE glfw glad spdlog debugbreak)
