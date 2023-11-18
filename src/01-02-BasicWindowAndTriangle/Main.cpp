#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>
#include <glm/vec3.hpp>

#include <expected>
#include <vector>
#include <string_view>
using namespace std::literals;

struct VertexPositionColor
{
    glm::vec3 Position;
    glm::vec3 Color;
};

std::expected<uint32_t, std::string> CreateProgram(
    uint32_t shaderType,
    std::string_view shaderSource)
{
    const char* shaderSourcePtr = shaderSource.data();
    auto shaderProgram = glCreateShaderProgramv(shaderType, 1, &shaderSourcePtr);
    auto linkStatus = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        auto infoLogLength = 0;
        char infoLog[1024];
        glGetProgramInfoLog(shaderProgram, 1024, &infoLogLength, infoLog);

        return std::unexpected(infoLog);
    }
    
    return shaderProgram;
}

void GLAPIENTRY DebugMessageCallback(
    GLenum source,
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

    spdlog::error("OpenGL Message: {} {}", type, debugMessageStream.str());
}

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

    int32_t framebufferWidth = 0;
    int32_t framebufferHeight = 0;
    glfwGetFramebufferSize(windowHandle, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glDebugMessageCallback(DebugMessageCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.35f, 0.76f, 0.16f, 1.0f);
    glClearDepthf(1.0f);

    auto label = "Default Framebuffer"sv;
    glObjectLabel(GL_FRAMEBUFFER, 0, label.size(), label.data());
    label = "Default InputLayout"sv;
    glObjectLabel(GL_VERTEX_ARRAY, 0, label.size(), label.data());

    std::array<VertexPositionColor, 3> vertices =
    {
        VertexPositionColor{ .Position = glm::vec3(-0.5f, +0.5f, 0.0f), .Color = glm::vec3(1.0f, 0.2f, 1.0f) },
        VertexPositionColor{ .Position = glm::vec3(+0.0f, -0.5f, 0.0f), .Color = glm::vec3(0.2f, 1.0f, 1.0f) },
        VertexPositionColor{ .Position = glm::vec3(+0.5f, +0.5f, 0.0f), .Color = glm::vec3(1.0f, 1.0f, 0.2f) }        
    };

    uint32_t vertexBuffer = 0;
    glCreateBuffers(1, &vertexBuffer);
    glNamedBufferStorage(vertexBuffer, vertices.size() * sizeof(VertexPositionColor), vertices.data(), GL_MAP_WRITE_BIT);

    uint32_t inputLayout = 0;
    glCreateVertexArrays(1, &inputLayout);

    glEnableVertexArrayAttrib(inputLayout, 0);
    glVertexArrayAttribFormat(inputLayout, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexPositionColor, Position));
    glVertexArrayAttribBinding(inputLayout, 0, 0);        

    glEnableVertexArrayAttrib(inputLayout, 1);
    glVertexArrayAttribFormat(inputLayout, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexPositionColor, Color));
    glVertexArrayAttribBinding(inputLayout, 1, 0);    

    glVertexArrayVertexBuffer(inputLayout, 0, vertexBuffer, 0, sizeof(VertexPositionColor));

    auto vertexShaderSource = R"glsl(
        #version 460 core

        out gl_PerVertex
        {
            vec4 gl_Position;
        };

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

    auto vertexShaderResult = CreateProgram(GL_VERTEX_SHADER, vertexShaderSource);
    if (!vertexShaderResult.has_value())
    {
        spdlog::error("OpenGL: Failed to compile vertex shader: {}", vertexShaderResult.error());
        return 0;
    }
    uint32_t vertexShader = vertexShaderResult.value();

    auto fragmentShaderResult = CreateProgram(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (!fragmentShaderResult.has_value())
    {
        spdlog::error("OpenGL: Failed to compile fragment shader: {}", fragmentShaderResult.error());
        return false;
    }
    uint32_t fragmentShader = fragmentShaderResult.value();

    uint32_t programPipeline = 0;
    glCreateProgramPipelines(1, &programPipeline);
    glUseProgramStages(programPipeline, GL_VERTEX_SHADER_BIT, vertexShader);
    glUseProgramStages(programPipeline, GL_FRAGMENT_SHADER_BIT, fragmentShader);

    glBindVertexArray(inputLayout);
    glBindProgramPipeline(programPipeline);

    glClearColor(0.7f, 0.1f, 0.4f, 1.0f);

    while (!glfwWindowShouldClose(windowHandle))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(windowHandle);
    }

    glDeleteProgram(fragmentShader);
    glDeleteProgram(vertexShader);
    glDeleteProgramPipelines(1, &programPipeline);

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &inputLayout);

    glfwDestroyWindow(windowHandle);
    glfwTerminate();

    return 0;
}