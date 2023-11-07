#pragma once

#include "../Shared/Application.hpp"
#include "VertexPositionUv.hpp"
#include "Program.hpp"

#include <vector>
#include <string>
#include <string_view>
#include <expected>

struct Program;

class HelloTriangleApplication final : public Application
{
protected:
    bool Load() override;
    void Unload() override;
    void Render() override;

private:
    std::expected<uint32_t, std::string> CreateShaderProgram(
        std::string_view label,
        uint32_t shaderType,
        std::string_view shaderSource);
    std::expected<Program, std::string> CreateProgram(
        std::string_view label,
        const std::string& vertexShaderFilePath,
        const std::string& fragmentShaderFilePath);

    uint32_t _inputLayout = 0;
    uint32_t _vertexBuffer = 0;
    uint32_t _indexBuffer = 0;
    std::vector<VertexPositionUv> _vertices;
    std::vector<uint32_t> _indices;

    Program _simpleProgram;
};