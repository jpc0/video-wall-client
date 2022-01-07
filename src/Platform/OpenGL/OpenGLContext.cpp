#include "OpenGLContext.h"

OpenGLContext::OpenGLContext(SDL_Window *windowHandle)
    : m_WindowHandle(windowHandle)
{
}

void OpenGLContext::Init()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    m_Context = SDL_GL_CreateContext(m_WindowHandle);
    int status = SDL_GL_MakeCurrent(m_WindowHandle, m_Context);
    SDL_assert((status == 0));

    gladLoadGL();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLContext::SwapBuffers()
{
    SDL_GL_SwapWindow(m_WindowHandle);
}

void OpenGLContext::SetVsync(bool enabled)
{
    if (enabled)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);
}