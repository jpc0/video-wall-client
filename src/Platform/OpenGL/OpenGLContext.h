#pragma once
#include "GraphicsContext.h"
#include <SDL2/SDL.h>
#include "glad.h"

class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(SDL_Window *windowHandle);
    void Init() override;
    void SwapBuffers() override;
    void SetVsync(bool enabled) override;

private:
    SDL_Window *m_WindowHandle;
    SDL_GLContext m_Context;
};