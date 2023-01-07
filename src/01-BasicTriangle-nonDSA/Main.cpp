#include <debugbreak.h>

#include <cstddef>
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <fstream>
#include <string>
#include <string_view>
#include <vector>

using namespace std::literals;

GLuint gVertexArrayPositionColor = {};
GLuint gVertexBuffer = {};
GLuint gIndexBuffer = {};
GLuint gProgram = {};

struct VertexPositionColor
{
    glm::vec3 Position;
    glm::vec3 Color;
};

std::vector<VertexPositionColor> gVertices =
{
    VertexPositionColor{ .Position = glm::vec3(-0.5f, +0.5f, 0.0f), .Color = glm::vec3(0.8f, 0.2f, 1.0f) },
    VertexPositionColor{ .Position = glm::vec3(+0.5f, +0.5f, 0.0f), .Color = glm::vec3(1.0f, 0.8f, 0.0f) },
    VertexPositionColor{ .Position = glm::vec3(+0.0f, -0.5f, 0.0f), .Color = glm::vec3(0.2f, 1.0f, 0.8f) }
};

std::vector<std::uint32_t> gIndices =
{
    0, 1, 2
};

bool CreateAndCompileShader(std::string_view vertexShaderSource, std::string_view fragmentShaderSource)
{
    int success = false;
    char log[1024] = {};
    const char* vertexShaderSourcePtr = vertexShaderSource.data();
    const auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSourcePtr, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 1024, nullptr, log);
        spdlog::error(log);
        return false;
    }

    const char* fragmentShaderSourcePtr = fragmentShaderSource.data();
    const auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSourcePtr, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 1024, nullptr, log);
        spdlog::error(log);
        return false;
    }

    gProgram = glCreateProgram();
    glAttachShader(gProgram, vertexShader);
    glAttachShader(gProgram, fragmentShader);
    glLinkProgram(gProgram);
    glGetProgramiv(gProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(gProgram, 1024, nullptr, log);
        spdlog::error(log);

        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void CleanupRender()
{
    glDeleteBuffers(1, &gVertexBuffer);
    glDeleteBuffers(1, &gIndexBuffer);
    glDeleteVertexArrays(1, &gVertexArrayPositionColor);

    glDeleteProgram(gProgram);
}

bool SetupRender()
{
    glGenVertexArrays(1, &gVertexArrayPositionColor);
    glBindVertexArray(gVertexArrayPositionColor);

    glGenBuffers(1, &gVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionColor) * gVertices.size(), gVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &gIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * gIndices.size(), gIndices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColor), (void*)offsetof(VertexPositionColor, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColor), (void*)offsetof(VertexPositionColor, Color));

    auto vertexShaderSource = R"glsl(
        #version 460 core

        layout (location = 0) in vec3 i_position;
        layout (location = 1) in vec3 i_color;

        layout (location = 0) out vec3 v_color;

        void main()
        {
            gl_Position = vec4(i_position, 1.0);
            v_color = i_color;
        })glsl"sv;

    auto fragmentShaderSource = R"glsl(
        #version 460 core

        layout (location = 0) in vec3 v_color;

        layout (location = 0) out vec4 out_color;

        void main()
        {
            out_color = vec4(v_color, 1.0);
        })glsl"sv;

    if (!CreateAndCompileShader(vertexShaderSource, fragmentShaderSource))
    {
        return false;
    }

    return true;
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gProgram);
    glBindVertexArray(gVertexArrayPositionColor);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void DebugCallback(GLenum source, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message, const void*)
{
    if (type == GL_DEBUG_TYPE_ERROR)
    {
        spdlog::error("GL: DEBUG CALLBACK type = {}, severity = error, message = {}\n", type, message);
        debug_break();
    }
}

int main(int, char**)
{
    if (glfwInit() == GLFW_FALSE)
    {
        spdlog::error("GLFW: Unable to initialize");
        return 1;
    }

    glfwSetErrorCallback([](auto errorCode, auto errorMessage)
    {
        spdlog::error("GLFW: Error {0}", errorMessage);
    });

    auto windowWidth = 1650;
    auto windowHeight = 1050;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    auto windowHandle = glfwCreateWindow(windowWidth, windowHeight, "Basic Triangle DSA", nullptr, nullptr);
    if (windowHandle == nullptr)
    {
        spdlog::error("GLFW: Unable to create window");
        glfwTerminate();
        return 1;
    }

    const auto primaryMonitor = glfwGetPrimaryMonitor();
    const auto primaryMonitorResolution = glfwGetVideoMode(primaryMonitor);
    glfwSetWindowPos(windowHandle, primaryMonitorResolution->width / 2 - windowWidth / 2, primaryMonitorResolution->height / 2 - windowHeight / 2);

    glfwMakeContextCurrent(windowHandle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(DebugCallback, nullptr);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    if (!SetupRender())
    {
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(windowHandle))
    {
        glfwPollEvents();

        Render();

        glfwSwapBuffers(windowHandle);
    }

    CleanupRender();

    glfwDestroyWindow(windowHandle);
    glfwTerminate();

    return 1;
}