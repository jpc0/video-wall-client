#pragma once
#include "GraphicsContext.h"
#include "glad.h"
#include <GLFW/glfw3.h>

class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(GLFWwindow *windowHandle);
    void Init() override;
    void SwapBuffers() override;
    void SetVsync(bool enabled) override;

private:
    GLFWwindow *m_WindowHandle;
};