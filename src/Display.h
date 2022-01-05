#pragma once

#include "Renderer.h"
#include "Configuration.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Display
{
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
        Renderer _renderer;
        AllScreenArray _wall;
        Shader *_shader;
        std::string _default_image_location;
        Screen _screen;
    };
}