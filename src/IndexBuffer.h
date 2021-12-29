#pragma once

#include <cstdint>

class IndexBuffer
{
public:
    IndexBuffer(const uint32_t *data, uint32_t count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline uint32_t GetCount() const { return _Count; }

private:
    uint32_t _RenderID;
    uint32_t _Count;
};