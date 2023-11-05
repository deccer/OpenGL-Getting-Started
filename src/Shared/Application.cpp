#include "Application.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void Application::Run()
{
    if (!Initialize())
    {
        return;
    }

    spdlog::info("App: Initialized");

    if (!Load())
    {
        return;
    }

    spdlog::info("App: Loaded");

    while (!glfwWindowShouldClose(_windowHandle))
    {
        glfwPollEvents();

        Update();

        Render();

        glfwSwapBuffers(_windowHandle);
    }

    spdlog::info("App: Unloading");

    Unload();

    spdlog::info("App: Unloaded");
}

bool Application::Initialize()
{
    if (glfwInit() == GLFW_FALSE)
    {
        spdlog::error("Glfw: Unable to initialize");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    auto primaryMonitor = glfwGetPrimaryMonitor();
    auto videoMode = glfwGetVideoMode(primaryMonitor);

    auto screenWidth = videoMode->width;
    auto screenHeight = videoMode->height;

    auto windowWidth = static_cast<int32_t>(static_cast<float>(screenWidth) * 0.8f);
    auto windowHeight = static_cast<int32_t>(static_cast<float>(screenHeight) * 0.8f);

    _windowHandle = glfwCreateWindow(windowWidth, windowHeight, "OpenGL - Getting Started", nullptr, nullptr);
    if (_windowHandle == nullptr)
    {
        const char* errorDescription = nullptr;
        auto errorCode = glfwGetError(&errorDescription);
        if (errorCode != GLFW_NO_ERROR)
        {
            spdlog::error("GLFW: Unable to create window Details_{}", errorDescription);
        }
        return false;
    }

    int32_t monitorLeft = 0;
    int32_t monitorTop = 0;
    glfwGetMonitorPos(primaryMonitor, &monitorLeft, &monitorTop);
    glfwSetWindowPos(_windowHandle, screenWidth / 2 - windowWidth / 2 + monitorLeft, screenHeight / 2 - windowHeight / 2 + monitorTop);

    glfwMakeContextCurrent(_windowHandle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    return true;
}

bool Application::Load()
{
    return true;
}

void Application::Unload()
{
    if (_windowHandle != nullptr)
    {
        glfwDestroyWindow(_windowHandle);
    }

    glfwTerminate();
}

void Application::Update()
{

}

void Application::Render()
{

}