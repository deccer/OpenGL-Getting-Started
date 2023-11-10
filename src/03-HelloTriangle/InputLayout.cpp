#include "InputLayout.hpp"

#include <glad/glad.h>

void InputLayout::AddVertexBufferBinding(
    uint32_t vertexBuffer,
    uint32_t bindingIndex,
    uint32_t offset,
    uint32_t stride)
{
    glVertexArrayVertexBuffer(Id, bindingIndex, vertexBuffer, offset, stride);
}

void InputLayout::AddIndexBufferBinding(uint32_t indexBuffer)
{
    glVertexArrayElementBuffer(Id, indexBuffer);
}

void InputLayout::Bind()
{
    glBindVertexArray(Id);
}