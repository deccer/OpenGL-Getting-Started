#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

int main(
    [[maybe_unused]] int32_t argc,
    [[maybe_unused]] char* argv[])
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

    auto windowHandle = glfwCreateWindow(windowWidth, windowHeight, "OpenGL - Getting Started", nullptr, nullptr);
    if (windowHandle == nullptr)
    {
        const char* errorDescription = nullptr;
        auto errorCode = glfwGetError(&errorDescription);
        if (errorCode != GLFW_NO_ERROR)
        {
            spdlog::error("GLFW: Unable to create window Details_{}", errorDescription);
        }
        return 0;
    }

    int32_t monitorLeft = 0;
    int32_t monitorTop = 0;
    glfwGetMonitorPos(primaryMonitor, &monitorLeft, &monitorTop);
    glfwSetWindowPos(windowHandle, screenWidth / 2 - windowWidth / 2 + monitorLeft, screenHeight / 2 - windowHeight / 2 + monitorTop);

    glfwSetFramebufferSizeCallback(windowHandle, [](
        [[maybe_unused]] GLFWwindow* window,
        int32_t framebufferWidth,
        int32_t framebufferHeight)
    {
        glViewport(0, 0, framebufferWidth, framebufferHeight);
    });

    glfwSetKeyCallback(windowHandle, [](
        GLFWwindow* window,
        int32_t key,
        [[maybe_unused]] int32_t scancode,
        int32_t action,
        [[maybe_unused]] int32_t mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });

    glfwMakeContextCurrent(windowHandle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.35f, 0.76f, 0.16f, 1.0f);
    glClearDepthf(1.0f);

    while (!glfwWindowShouldClose(windowHandle))
    {
        glfwPollEvents();

        glfwSwapBuffers(windowHandle);
    }

    glfwDestroyWindow(windowHandle);
    glfwTerminate();

    return 0;
}