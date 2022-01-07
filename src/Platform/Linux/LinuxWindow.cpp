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
        uint32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP;
        m_Window = SDL_CreateWindow(
            props.Title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            (int)props.Width,
            (int)props.Height,
            window_flags);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    m_Context = SDL_GL_CreateContext(m_Window);
    SDL_GL_MakeCurrent(m_Window, m_Context);
    gladLoadGL();
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SetVSync(true);
}

void LinuxWindow::Shutdown()
{
    SDL_DestroyWindow(m_Window);
}

void LinuxWindow::OnUpdate()
{
    SDL_GL_SwapWindow(m_Window);
}

void LinuxWindow::SetVSync(bool enabled)
{
    if (enabled)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);

    m_Data.VSync = enabled;
}

bool LinuxWindow::IsVSync() const
{
    return m_Data.VSync;
}
