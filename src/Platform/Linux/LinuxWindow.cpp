#include "LinuxWindow.h"

static uint8_t s_GLFWInitialized = false;

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

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

    glfwSetErrorCallback(error_callback);

    if (!s_GLFWInitialized)
    {
        if (!glfwInit())
        {
            exit(EXIT_FAILURE);
        }

        s_GLFWInitialized = true;
    }

    {
        m_Window = glfwCreateWindow(
            (int)props.Width,
            (int)props.Height,
            props.Title.c_str(),
            glfwGetPrimaryMonitor(),
            nullptr);
    }
    glfwSetKeyCallback(m_Window, key_callback);
    m_Context = new OpenGLContext(m_Window);
    m_Context->Init();
    SetVSync(true);
}

void LinuxWindow::Shutdown()
{
    glfwDestroyWindow(m_Window);
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
