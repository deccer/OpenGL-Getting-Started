#pragma once

#include <cstdint>
#include <vector>
#include <string_view>

struct InputLayoutElement
{
    uint32_t AttributeIndex;
    uint32_t ComponentCount;
    uint32_t ComponentType;
    bool IsNormalized = false;
    uint32_t Offset;
    uint32_t BindingIndex;
};

struct InputLayout
{
    void AddVertexBufferBinding(
        uint32_t vertexBuffer,
        uint32_t bindingIndex,
        uint32_t offset,
        uint32_t stride);
    void AddIndexBufferBinding(uint32_t indexBuffer);
    void Bind();

    uint32_t Id = 0;
    std::string_view Label;
};