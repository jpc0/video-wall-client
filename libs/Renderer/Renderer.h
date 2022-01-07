#pragma once
#include "glad.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "signal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <exception>
#include "Configuration.h"

#define ASSERT(x) \
    if (!(x))     \
    {             \
    }
#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char *function, const char *file, int line);

class Renderer
{
public:
public:
    Renderer(const Configuration::ConfigData &configuration);
    ~Renderer() = default;
    void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;
    void Clear() const;
};
