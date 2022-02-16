#include "VertexBuffer.h"
#include <cstdint>
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void *data, uint32_t size)
/* Size in bytes */
{
    glGenBuffers(1, &_RenderID);
    glBindBuffer(GL_ARRAY_BUFFER, _RenderID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_RenderID);
}

void VertexBuffer::Bind() const
{

    glBindBuffer(GL_ARRAY_BUFFER, _RenderID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}