#pragma once

#include "../Shared/Application.hpp"
#include "VertexPositionUv.hpp"

#include <vector>
#include <string>
#include <string_view>
#include <expected>

class HelloTriangleApplication final : public Application
{
protected:
    bool Load() override;
    void Render() override;

private:
    uint32_t _vao = 0;
    uint32_t _vertexBuffer = 0;
    uint32_t _indexBuffer = 0;
    std::vector<VertexPositionUv> _vertices;

    uint32_t _simpleProgram = 0;

    std::expected<uint32_t, std::string> CreateShaderProgram(
        std::string_view label,
        uint32_t shaderType,
        std::string_view shaderSource);
    std::expected<uint32_t, std::string> CreateShaderProgramPipeline(
        std::string_view label,
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath);
};