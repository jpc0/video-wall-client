#pragma once
#include <cstdint>
#include <string>

class Texture
{
public:
    Texture(const std::string &path);
    ~Texture();

    void Bind(uint32_t slot = 0) const;
    void Unbind() const;

    inline int GetWidth() const { return _Width; }
    inline int GetHeight() const { return _Height; }

private:
    uint32_t _RendererID;
    std::string _FilePath;
    uint8_t *_LocalBuffer;
    int32_t _Width, _Height, _BPP;
};