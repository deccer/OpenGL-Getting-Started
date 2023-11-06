#include "HelloTriangleApplication.hpp"

#include <glad/glad.h>
#include <format>

bool HelloTriangleApplication::Load()
{

    return true;
}

void HelloTriangleApplication::Render()
{

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

std::expected<uint32_t, std::string> HelloTriangleApplication::CreateShaderProgramPipeline(
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

    auto vertexShaderProgram = CreateShaderProgram(std::format("VS_{}", label), GL_VERTEX_SHADER, vertexShaderFileContent.value());
    if (!vertexShaderProgram.has_value())
    {
        return std::unexpected(vertexShaderProgram.error());
    }

    auto fragmentShaderProgram = CreateShaderProgram(std::format("FS_{}", label), GL_FRAGMENT_SHADER, fragmentShaderFileContent.value());
    if (!fragmentShaderProgram.has_value())
    {
        return std::unexpected(fragmentShaderProgram.error());
    }

    auto programPipeline = 0u;
    glCreateProgramPipelines(1, &programPipeline);

    glUseProgramStages(programPipeline, GL_VERTEX_SHADER_BIT, vertexShaderProgram.value());
    glUseProgramStages(programPipeline, GL_FRAGMENT_SHADER_BIT, fragmentShaderProgram.value());
    
    return programPipeline;
}