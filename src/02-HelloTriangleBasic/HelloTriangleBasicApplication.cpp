#include "HelloTriangleBasicApplication.hpp"
#include "VertexPositionColor.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <array>
#include <expected>
#include <format>
#include <vector>

bool HelloTriangleBasicApplication::Load()
{
    if (!Application::Load())
    {
        return false;
    }

    auto vertexShaderFileContent = ReadTextFromFile("Data/Shaders/Simple.vs.glsl");
    if (!vertexShaderFileContent.has_value())
    {
        spdlog::error("App: Failed to read vertex shader from file: {}", vertexShaderFileContent.error());
        return false;
    }

    auto vertexShaderSource = vertexShaderFileContent.value().data();
    
    auto fragmentShaderFileContent = ReadTextFromFile("Data/Shaders/Simple.fs.glsl");
    if (!fragmentShaderFileContent.has_value())
    {
        spdlog::error("App: Failed to read from file. {}", fragmentShaderFileContent.error());
        return false;
    }

    auto fragmentShaderSource = fragmentShaderFileContent.value().data();

    _simpleProgramVertexShader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vertexShaderSource);
    glProgramParameteri(_simpleProgramVertexShader, GL_PROGRAM_SEPARABLE, GL_TRUE);
    auto linkStatus = 0;
    glGetProgramiv(_simpleProgramVertexShader, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE)
    {
        auto infoLogLength = 0;
        char infoLog[1024];
        glGetProgramInfoLog(_simpleProgramVertexShader, 1024, &infoLogLength, infoLog);

        spdlog::error("App: Failed to compile vertex shader: {}", infoLog);
        return false;
    }

    std::string_view label = "VertexShader_Simple";
    glObjectLabel(GL_PROGRAM, _simpleProgramVertexShader, label.size(), label.data());

    _simpleProgramFragmentShader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragmentShaderSource);
    glProgramParameteri(_simpleProgramFragmentShader, GL_PROGRAM_SEPARABLE, GL_TRUE);
    linkStatus = 0;
    glGetProgramiv(_simpleProgramFragmentShader, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE)
    {
        auto infoLogLength = 0;
        char infoLog[1024];
        glGetProgramInfoLog(_simpleProgramFragmentShader, 1024, &infoLogLength, infoLog);

        spdlog::error("App: Failed to compile fragment shader: {}", infoLog);
        return false;
    }

    label = "FragmentShader_Simple";
    glObjectLabel(GL_PROGRAM, _simpleProgramFragmentShader, label.size(), label.data());

    glCreateProgramPipelines(1, &_simpleProgramPipeline);
    label = "ProgramPipeline_Simple";
    glObjectLabel(GL_PROGRAM_PIPELINE, _simpleProgramPipeline, label.size(), label.data());

    glUseProgramStages(_simpleProgramPipeline, GL_VERTEX_SHADER_BIT, _simpleProgramVertexShader);
    glUseProgramStages(_simpleProgramPipeline, GL_FRAGMENT_SHADER_BIT, _simpleProgramFragmentShader);

    glCreateVertexArrays(1, &_inputLayout);
    label = "InputLayout_Simple";
    glObjectLabel(GL_VERTEX_ARRAY, _inputLayout, label.size(), label.data());

    glEnableVertexArrayAttrib(_inputLayout, 0);
    glVertexArrayAttribFormat(_inputLayout, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexPositionColor, Position));
    glVertexArrayAttribBinding(_inputLayout, 0, 0);

    glEnableVertexArrayAttrib(_inputLayout, 1);
    glVertexArrayAttribFormat(_inputLayout, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexPositionColor, Color));
    glVertexArrayAttribBinding(_inputLayout, 1, 0);

    std::array<VertexPositionColor, 3> vertices =
    {
        VertexPositionColor{ .Position = glm::vec3(-0.5f, +0.5f, 0.0f), .Color = glm::vec3(1.0f, 0.2f, 1.0f) },
        VertexPositionColor{ .Position = glm::vec3(+0.0f, -0.5f, 0.0f), .Color = glm::vec3(0.2f, 1.0f, 1.0f) },
        VertexPositionColor{ .Position = glm::vec3(+0.5f, +0.5f, 0.0f), .Color = glm::vec3(1.0f, 1.0f, 0.2f) }        
    };

    glCreateBuffers(1, &_vertexBuffer);
    label = "VertexBuffer_Triangle";
    glObjectLabel(GL_BUFFER, _vertexBuffer, label.size(), label.data());
    glNamedBufferData(_vertexBuffer, vertices.size() * sizeof(VertexPositionColor), vertices.data(), GL_STATIC_DRAW);

    std::array<uint32_t, 3> indices =
    {
        0,
        1,
        2
    };

    glCreateBuffers(1, &_indexBuffer);
    label = "IndexBuffer_Triangle";
    glObjectLabel(GL_BUFFER, _vertexBuffer, label.size(), label.data());
    glNamedBufferData(_indexBuffer, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(_inputLayout, 0, _vertexBuffer, 0, sizeof(VertexPositionColor));
    glVertexArrayElementBuffer(_inputLayout, _indexBuffer);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    return true;
}

void HelloTriangleBasicApplication::Unload()
{
    glDeleteProgram(_simpleProgramVertexShader);
    glDeleteProgram(_simpleProgramFragmentShader);
    glDeleteProgramPipelines(1, &_simpleProgramPipeline);
    Application::Unload();
}

void HelloTriangleBasicApplication::Render()
{
    Application::Render();

    glBindVertexArray(_inputLayout);
    glBindProgramPipeline(_simpleProgramPipeline);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
}
