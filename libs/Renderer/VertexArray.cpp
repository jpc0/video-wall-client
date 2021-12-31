#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &_RendererID);
}
VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout)
{
    Bind();
    vb.Bind();
    const auto &elements = layout.GetElements();
    uint32_t offset = 0;
    for (uint32_t i = 0; i < elements.size(); i++)
    {
        const auto &element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void *)offset); // This throws a warning, it's fine as long at offset is unsigned
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

void VertexArray::Bind() const
{
    glBindVertexArray(_RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}