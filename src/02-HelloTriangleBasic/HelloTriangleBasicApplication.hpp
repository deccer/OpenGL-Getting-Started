#pragma once

#include "../Shared/Application.hpp"

class HelloTriangleBasicApplication final : public Application
{
protected:
    bool Load() override;
    void Unload() override;
    void Render() override;

private:
    uint32_t _inputLayout = 0;
    uint32_t _vertexBuffer = 0;
    uint32_t _indexBuffer = 0;

    uint32_t _simpleProgramPipeline = 0;
    uint32_t _simpleProgramVertexShader = 0;
    uint32_t _simpleProgramFragmentShader = 0;
};