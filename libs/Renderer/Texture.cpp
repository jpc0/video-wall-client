#include "Texture.h"
#include "glad.h"

#include "stb_image.h"

Texture::Texture(const std::string &path) : _FilePath{path}, _LocalBuffer{nullptr}, _Width{0}, _Height{0}, _BPP{0}
{
    stbi_set_flip_vertically_on_load(1);
    _LocalBuffer = stbi_load(path.c_str(), &_Width, &_Height, &_BPP, 4);

    glGenTextures(1, &_RendererID);
    glBindTexture(GL_TEXTURE_2D, _RendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _LocalBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (_LocalBuffer)
        stbi_image_free(_LocalBuffer);
}

Texture::Texture(Frame &frame) : _FilePath{""}, _LocalBuffer{frame.buffer}, _Width{frame.width}, _Height{frame.height}, _BPP{frame.bpp}
{
    glGenTextures(1, &_RendererID);
    glBindTexture(GL_TEXTURE_2D, _RendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _LocalBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {}

void Texture::Bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _RendererID);
}
void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}