#include "Application.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

class ApplicationAccess final
{
public:
    static void FramebufferResizeCallback(
        GLFWwindow* window,
        int32_t framebufferWidth,
        int32_t framebufferHeight)
    {
        auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (application == nullptr)
        {
            spdlog::error("App: You forgot to call glfwSetWindowUserPointer in Application::Initialize");
            return;
        }

        if (framebufferWidth > 0 && framebufferHeight > 0)
        {
            application->framebufferWidth = framebufferWidth;
            application->framebufferHeight = framebufferHeight;
            application->OnFramebufferResized();
            application->Render();
        }
    }

    static void KeyCallback(
        GLFWwindow* window,
        int32_t key,
        int32_t scancode,
        int32_t action,
        int32_t modifiers
    )
    {
        auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (application == nullptr)
        {
            spdlog::error("App: You forgot to call glfwSetWindowUserPointer in Application::Initialize");
            return;
        }

        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            application->OnKeyDown(key, modifiers, scancode);
        }
        else
        {
            application->OnKeyUp(key, modifiers, scancode);
        }
    }
};

Application::~Application()
{

}

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

    glfwSetWindowUserPointer(_windowHandle, this);

    int32_t monitorLeft = 0;
    int32_t monitorTop = 0;
    glfwGetMonitorPos(primaryMonitor, &monitorLeft, &monitorTop);
    glfwSetWindowPos(_windowHandle, screenWidth / 2 - windowWidth / 2 + monitorLeft, screenHeight / 2 - windowHeight / 2 + monitorTop);

    glfwSetFramebufferSizeCallback(_windowHandle, ApplicationAccess::FramebufferResizeCallback);
    glfwSetKeyCallback(_windowHandle, ApplicationAccess::KeyCallback);

    glfwMakeContextCurrent(_windowHandle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glClearColor(0.35f, 0.76f, 0.16f, 1.0f);
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void Application::OnFramebufferResized()
{
    spdlog::info("Framebuffer resized to {}_{}", framebufferWidth, framebufferHeight);
}

void Application::OnKeyDown(
    int32_t key,
    int32_t modifiers,
    int32_t scancode)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(_windowHandle, GLFW_TRUE);
    }

    if (key == GLFW_KEY_F11)
    {
        ToggleFullscreen();
    }
}

void Application::OnKeyUp(
    int32_t key,
    int32_t modifiers,
    int32_t scancode)
{
}

void Application::ToggleFullscreen()
{
    _isFullscreen = !_isFullscreen;

    glfwSetWindowAttrib(_windowHandle, GLFW_DECORATED, _isFullscreen ? GLFW_FALSE : GLFW_TRUE);
    glfwSetWindowAttrib(_windowHandle, GLFW_RESIZABLE, _isFullscreen ? GLFW_FALSE : GLFW_TRUE);

    int32_t monitorLeft = 0;
    int32_t monitorTop = 0;
    auto primaryMonitor = glfwGetPrimaryMonitor();
    auto videoMode = glfwGetVideoMode(primaryMonitor);

    auto screenWidth = videoMode->width;
    auto screenHeight = videoMode->height;

    glfwGetMonitorPos(primaryMonitor, &monitorLeft, &monitorTop);

    if (_isFullscreen)
    {
        glfwSetWindowPos(_windowHandle, monitorLeft, monitorTop);
        glfwSetWindowSize(_windowHandle, screenWidth, screenHeight);
    }
    else
    {
        auto windowWidth = static_cast<int32_t>(static_cast<float>(screenWidth) * 0.8f);
        auto windowHeight = static_cast<int32_t>(static_cast<float>(screenHeight) * 0.8f);

        glfwSetWindowPos(_windowHandle, screenWidth / 2 - windowWidth / 2 + monitorLeft, screenHeight / 2 - windowHeight / 2 + monitorTop);
        glfwSetWindowSize(_windowHandle, windowWidth, windowHeight);
    }
}