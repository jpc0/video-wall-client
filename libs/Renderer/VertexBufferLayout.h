#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBufferElement
{
    uint32_t type;
    uint32_t count;
    uint8_t normalized;
    static uint32_t GetSizeOfType(uint32_t type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return 4;
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        }
        return 0;
    }
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> _Elements;
    uint32_t _Stride;

public:
    VertexBufferLayout() : _Stride{0} {}
    template <typename T>
    void
    Push(uint32_t count)
    {
        // static_assert(false);
    }

    inline const std::vector<VertexBufferElement> GetElements() const { return _Elements; }
    inline uint32_t GetStride() const { return _Stride; }
};

template <>
inline void VertexBufferLayout::Push<float>(uint32_t count)
{
    _Elements.push_back({GL_FLOAT, count, GL_FALSE});
    _Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
}
template <>
inline void VertexBufferLayout::Push<uint32_t>(uint32_t count)
{
    _Elements.push_back({GL_UNSIGNED_INT, count, GL_TRUE});
    _Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
}
template <>
inline void VertexBufferLayout::Push<uint8_t>(uint32_t count)
{
    _Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    _Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
}
