#pragma once

#include "Renderer.h"
#include "Configuration.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Display
{
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
        void DisplaySingleImage(const Configuration::ConfigData &configuration);
        bool ShouldExit();

    private:
        Renderer _renderer;
        AllScreenArray _screen;
    };
}