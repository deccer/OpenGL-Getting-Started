#include "HelloTriangleApplication.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <format>

bool HelloTriangleApplication::Load()
{
    if (!Application::Load())
    {
        return false;
    }

    auto createProgramResult = CreateProgram(
        "Simple",
        "Data/Shaders/Simple.vs.glsl",
        "Data/Shaders/Simple.fs.glsl");
    if (!createProgramResult.has_value())
    {
        spdlog::error("Building Program {} failed. {}", "Simple", createProgramResult.error());
        return false;
    }

    _simpleProgram = createProgramResult.value();

    _inputLayout = CreateInputLayout("PositionUv", std::to_array<const InputLayoutElement>(
    {
        { .AttributeIndex = 0, .ComponentCount = 3, .ComponentType = GL_FLOAT, .IsNormalized = GL_FALSE, .Offset = offsetof(VertexPositionUv, Position), .BindingIndex = 0 },
        { .AttributeIndex = 1, .ComponentCount = 2, .ComponentType = GL_FLOAT, .IsNormalized = GL_FALSE, .Offset = offsetof(VertexPositionUv, Uv), .BindingIndex = 0 },
    }));

    _vertices.push_back({.Position = glm::vec3(-0.5f, +0.5f, 0.0f), .Uv = glm::vec2(0.0f, 1.0f)});
    _vertices.push_back({.Position = glm::vec3(+0.5f, +0.5f, 0.0f), .Uv = glm::vec2(1.0f, 1.0f)});
    _vertices.push_back({.Position = glm::vec3(+0.0f, -0.5f, 0.0f), .Uv = glm::vec2(0.5f, 0.0f)});

    glCreateBuffers(1, &_vertexBuffer);
    glNamedBufferData(_vertexBuffer, _vertices.size() * sizeof(VertexPositionUv), _vertices.data(), GL_STATIC_DRAW);

    _indices.push_back(0);
    _indices.push_back(1);
    _indices.push_back(2);

    glCreateBuffers(1, &_indexBuffer);
    glNamedBufferData(_indexBuffer, _indices.size() * sizeof(uint32_t), _indices.data(), GL_STATIC_DRAW);

    _inputLayout.AddVertexBufferBinding(_vertexBuffer, 0, 0, sizeof(VertexPositionUv));
    _inputLayout.AddIndexBufferBinding(_indexBuffer);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    return true;
}

void HelloTriangleApplication::Unload()
{
    glDeleteProgram(_simpleProgram.VertexShader);
    glDeleteProgram(_simpleProgram.FragmentShader);
    glDeleteProgramPipelines(1, &_simpleProgram.Id);
    Application::Unload();
}

void HelloTriangleApplication::Render()
{
    Application::Render();

    _inputLayout.Bind();
    glBindProgramPipeline(_simpleProgram.Id);

    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr);
}

std::expected<uint32_t, std::string> HelloTriangleApplication::CreateShaderProgram(
        std::string_view label,
        uint32_t shaderType,
        std::string_view shaderSource)
{
    auto shaderContent = shaderSource.data();
    auto shaderProgram = glCreateShaderProgramv(shaderType, 1, &shaderContent);
    glProgramParameteri(shaderProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    auto linkStatus = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE)
    {
        auto infoLogLength = 0;
        char infoLog[1024];
        glGetProgramInfoLog(shaderProgram, 1024, &infoLogLength, infoLog);

        return std::unexpected(infoLog);
    }

    glObjectLabel(GL_PROGRAM, shaderProgram, label.size(), label.data());

    return shaderProgram;
}

std::expected<Program, std::string> HelloTriangleApplication::CreateProgram(
    std::string_view label,
    const std::string& vertexShaderFilePath,
    const std::string& fragmentShaderFilePath)
{
    auto vertexShaderFileContent = ReadTextFromFile(vertexShaderFilePath.data());
    if (!vertexShaderFileContent.has_value())
    {
        return std::unexpected(vertexShaderFileContent.error());
    }

    auto fragmentShaderFileContent = ReadTextFromFile(fragmentShaderFilePath.data());
    if (!fragmentShaderFileContent.has_value())
    {
        return std::unexpected(fragmentShaderFileContent.error());
    }

    Program program = {};

    auto vertexShaderProgram = CreateShaderProgram(std::format("VS_{}", label), GL_VERTEX_SHADER, vertexShaderFileContent.value());
    if (!vertexShaderProgram.has_value())
    {
        return std::unexpected(vertexShaderProgram.error());
    }

    program.VertexShader = vertexShaderProgram.value();

    auto fragmentShaderProgram = CreateShaderProgram(std::format("FS_{}", label), GL_FRAGMENT_SHADER, fragmentShaderFileContent.value());
    if (!fragmentShaderProgram.has_value())
    {
        return std::unexpected(fragmentShaderProgram.error());
    }

    program.FragmentShader = fragmentShaderProgram.value();

    glCreateProgramPipelines(1, &program.Id);

    glUseProgramStages(program.Id, GL_VERTEX_SHADER_BIT, program.VertexShader);
    glUseProgramStages(program.Id, GL_FRAGMENT_SHADER_BIT, program.FragmentShader);

    return program;
}

InputLayout HelloTriangleApplication::CreateInputLayout(
    std::string_view label,
    std::span<const InputLayoutElement> elements)
{
    InputLayout inputLayout = {};
    inputLayout.Label = label;
    glCreateVertexArrays(1, &inputLayout.Id);
    glObjectLabel(GL_VERTEX_ARRAY, inputLayout.Id, label.size(), label.data());

    for(auto& element : elements)
    {
        glEnableVertexArrayAttrib(inputLayout.Id, element.AttributeIndex);
        glVertexArrayAttribFormat(inputLayout.Id, element.AttributeIndex, element.ComponentCount, element.ComponentType, element.IsNormalized, element.Offset);
        glVertexArrayAttribBinding(inputLayout.Id, element.AttributeIndex, element.BindingIndex);
    }

    return inputLayout;
}
