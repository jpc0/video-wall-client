#pragma once

#include <memory>
#include "Renderer.h"
#include "Window.h"
#include "Configuration.h"
#include <memory>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Display
{
    struct Image
    {
        std::unique_ptr<VertexArray> va;
        std::unique_ptr<IndexBuffer> ib;
        std::unique_ptr<Shader> shader;
        std::unique_ptr<VertexBuffer> vb;
        std::unique_ptr<VertexBufferLayout> layout;
        std::unique_ptr<Texture> texture;
    };
    struct Screen
    {
        float width;
        float height;
        float l_bezel;
        float r_bezel;
        float t_bezel;
        float b_bezel;
        int h_index;
        int v_index;
    };

    class AllScreenArray
    {
    public:
        AllScreenArray(const Configuration::ConfigData &configuration);
        ~AllScreenArray() = default;
        float GetAspectRatio();

    public:
        float _total_width;
        float _total_height;
    };

    class Display
    {
    public:
        Display(const Configuration::ConfigData &configuration);
        ~Display();
        void DisplaySingleImage(const std::string &image_location);
        inline void DisplayDefaultImage() { DisplaySingleImage(_default_image_location); }
        bool ShouldExit();

    private:
        std::unique_ptr<Window> _window;
        Renderer _renderer;
        AllScreenArray _wall;
        std::string _default_image_location;
        Screen _screen;
        Image _current_image;
    };
}