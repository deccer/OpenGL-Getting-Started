#include "Application.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <debugbreak.h>

#include <format>
#include <fstream>

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

    static void GLAPIENTRY DebugMessageCallback(GLenum source,
                                        GLenum type,
                                        GLuint id,
                                        GLenum severity,
                                        [[maybe_unused]] GLsizei length,
                                        const GLchar* message,
                                        [[maybe_unused]] const void* userParam)
    {
        // Ignore certain verbose info messages (particularly ones on Nvidia).
        if (id == 131169 || 
            id == 131185 || // NV: Buffer will use video memory
            id == 131218 || 
            id == 131204 || // Texture cannot be used for texture mapping
            id == 131222 ||
            id == 131154 || // NV: pixel transfer is synchronized with 3D rendering
            id == 0         // gl{Push, Pop}DebugGroup
        )
        return;

        std::stringstream debugMessageStream;
        debugMessageStream << message << '\n';

        switch (source)
        {
            case GL_DEBUG_SOURCE_API: debugMessageStream << "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: debugMessageStream << "Source: Window Manager"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: debugMessageStream << "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY: debugMessageStream << "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION: debugMessageStream << "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER: debugMessageStream << "Source: Other"; break;
        }

        debugMessageStream << '\n';

        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: debugMessageStream << "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: debugMessageStream << "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: debugMessageStream << "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY: debugMessageStream << "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE: debugMessageStream << "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER: debugMessageStream << "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP: debugMessageStream << "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP: debugMessageStream << "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER: debugMessageStream << "Type: Other"; break;
        }

        debugMessageStream << '\n';

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH: debugMessageStream << "Severity: high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM: debugMessageStream << "Severity: medium"; break;
            case GL_DEBUG_SEVERITY_LOW: debugMessageStream << "Severity: low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: debugMessageStream << "Severity: notification"; break;
        }

        if (userParam != nullptr)
        {
            auto windowHandle = (GLFWwindow*)userParam;
            auto application = static_cast<Application*>(glfwGetWindowUserPointer(windowHandle));
            if (application == nullptr)
            {
                spdlog::error("App: You forgot to call glfwSetWindowUserPointer in Application::Initialize");
                return;
            }

            application->OnOpenGLDebugMessage(type, debugMessageStream.str());
        }
    }
};

Application::~Application()
{

}

std::expected<std::string, std::string> Application::ReadTextFromFile(std::string_view filePath)
{
    std::ifstream file(filePath.data(), std::ios::ate);
    if (file.bad())
    {
        return std::unexpected(std::format("Io: Unable to read from file {}", filePath));
    }
    auto fileSize = file.tellg();
    if (fileSize == 0)
    {
        return std::unexpected(std::format("Io: File {} is empty", filePath));
    }

    std::string result(fileSize, '\0');
    file.seekg(0);
    file.read((char*)result.data(), result.size());
    return result;
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

    glDebugMessageCallback(ApplicationAccess::DebugMessageCallback, _windowHandle);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.35f, 0.76f, 0.16f, 1.0f);
    glClearDepthf(1.0f);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::OnFramebufferResized()
{
    spdlog::info("Framebuffer resized to {}_{}", framebufferWidth, framebufferHeight);

    glViewport(0, 0, framebufferWidth, framebufferHeight);
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

void Application::OnOpenGLDebugMessage(uint32_t messageType, std::string_view debugMessage)
{
    spdlog::error(debugMessage);
    if (messageType == GL_DEBUG_TYPE_ERROR)
    {
        debug_break();
    }
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

    Render();
}