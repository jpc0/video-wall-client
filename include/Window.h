#pragma once
#include <sstream>

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

struct WindowProps
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;

    WindowProps(const std::string &title = "Video Wall",
                uint32_t width = 1280,
                uint32_t height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};

// Interface representing a desktop system based Window
class Window
{
public:
    virtual ~Window() = default;

    virtual void OnUpdate() = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    // Window attributes
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    static Window *Create(const WindowProps &props = WindowProps());
};
