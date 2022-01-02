#include "Renderer.h"
#include <iostream>
#include "Configuration.h"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool GLLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer(Configuration::ConfigData &configuration)
{
    /* Initialize the library */
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Failed to init Video, error: " << SDL_GetError();
        throw INITFAIL{};
    }

    uint32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP;

    /* Create a windowed mode window and its OpenGL context */
    window = SDL_CreateWindow(
        "Video Wall Client",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        configuration.width,
        configuration.height,
        window_flags);

    if (!window)
    {
        std::cout << "Could not create windown: " << SDL_GetError() << std::endl;
        throw WINDOWCREATEFAIL{};
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    context = SDL_GL_CreateContext(window);

    gladLoadGL();

    /* Make the window's context current */
    SDL_GL_MakeCurrent(window, context);

    SDL_GL_SetSwapInterval(1);
    glEnable(GL_TEXTURE_2D);

    // Set alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}