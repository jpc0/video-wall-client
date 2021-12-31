#include "IndexBuffer.h"
#include <cstdint>
#include "Renderer.h"

IndexBuffer::IndexBuffer(const uint32_t *data, uint32_t count) : _Count{count}
{
    glGenBuffers(1, &_RenderID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _RenderID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &_RenderID);
}

void IndexBuffer::Bind() const
{

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _RenderID);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}