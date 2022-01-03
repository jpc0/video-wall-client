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

struct INITFAIL : public std::exception
{
    const char *what() const throw()
    {
        return "Failed to init Video";
    }
};

struct WINDOWCREATEFAIL : public std::exception
{
    const char *what() const throw()
    {
        return "Failed to create window";
    }
};

class Renderer
{
public:
    SDL_Window *window;
    SDL_GLContext context;

public:
    Renderer(const Configuration::ConfigData &configuration);
    ~Renderer() = default;
    void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;
    void Clear() const;
};
