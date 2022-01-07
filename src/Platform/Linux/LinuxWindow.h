#pragma once
#include "Window.h"
#include <iostream>
#include "OpenGLContext.h"
#include <SDL2/SDL.h>

class LinuxWindow : public Window
{
public:
    LinuxWindow(const WindowProps &props);
    virtual ~LinuxWindow();

    void OnUpdate() override;

    unsigned int GetWidth() const override { return m_Data.Width; }
    unsigned int GetHeight() const override { return m_Data.Height; }

    // Window attributes
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

private:
    virtual void Init(const WindowProps &props);
    virtual void Shutdown();

private:
    SDL_Window *m_Window;
    GraphicsContext *m_Context;

    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;
    };

    WindowData m_Data;
};