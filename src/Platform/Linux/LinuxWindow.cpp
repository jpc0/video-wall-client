#include "LinuxWindow.h"

static uint8_t s_SDLInitialized = false;

Window *Window::Create(const WindowProps &props)
{
    return new LinuxWindow(props);
}

LinuxWindow::LinuxWindow(const WindowProps &props)
{
    Init(props);
}

LinuxWindow::~LinuxWindow()
{
    Shutdown();
}

void LinuxWindow::Init(const WindowProps &props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    if (!s_SDLInitialized)
    {
        int success = (SDL_Init(SDL_INIT_VIDEO) == 0);
        if (!success)
        {
            std::cout << "Failed to init Video, error: " << SDL_GetError();
            throw INITFAIL{};
        }

        s_SDLInitialized = true;
    }

    {
        uint32_t _window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_FOCUS;
        _window_flags |= SDL_WINDOW_OPENGL;
        m_Window = SDL_CreateWindow(
            props.Title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            (int)props.Width,
            (int)props.Height,
            _window_flags);
    }
    m_Context = new OpenGLContext(m_Window);
    m_Context->Init();
    SetVSync(true);
}

void LinuxWindow::Shutdown()
{
    SDL_DestroyWindow(m_Window);
    delete m_Context;
}

void LinuxWindow::OnUpdate()
{
    m_Context->SwapBuffers();
}

void LinuxWindow::SetVSync(bool enabled)
{
    m_Context->SetVsync(enabled);

    m_Data.VSync = enabled;
}

bool LinuxWindow::IsVSync() const
{
    return m_Data.VSync;
}
