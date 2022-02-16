#pragma once

#include <memory>
#include "Renderer.h"
#include "Window.h"
#include "Configuration.h"
#include <memory>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <atomic>
#include "VideoPlayer.h"

namespace Display
{
    struct TexturedQuad
    {
        float x_origin;
        float y_origin;
        float x_end;
        float y_end;
        float x_texture_origin{0.0f};
        float y_texture_origin{0.0f};
        float x_texture_end{1.0f};
        float y_texture_end{1.0f};
    };
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

        float brightness{0};
        float r_brightness{0};
        float g_brightness{0};
        float b_brightness{0};

        float contrast{1};
        float r_contrast{1};
        float g_contrast{1};
        float b_contrast{1};

        float gamma{1};
        float r_gamma{1};
        float g_gamma{1};
        float b_gamma{1};
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

    enum PlayState
    {
        pImage,
        pVideo,
    };

    class Display
    {
    public:
        Display(const Configuration::ConfigData &configuration);
        ~Display();
        void DisplaySingleImage(const std::string &image_location);
        void PlayVideo(Frame &frame, float framerate);
        void UpdateVideoFrame(Frame &frame);
        void ProcessColour();
        void GenerateQuad();
        void Refresh();

        inline void DisplayDefaultImage() { DisplaySingleImage(_default_image_location); }

        /****************** Brightness Funcs **********************************/
        inline float AdjustBrightness(float adj)
        {
            _screen.brightness += adj;
            _current_image.shader->SetUniform1f("u_brightness", _screen.brightness);

            return _screen.brightness;
        }
        inline float AdjustBrightnessr(float adj)
        {
            _screen.r_brightness += adj;
            _current_image.shader->SetUniform1f("u_rbrightness", _screen.r_brightness);

            return _screen.r_brightness;
        }
        inline float AdjustBrightnessg(float adj)
        {
            _screen.g_brightness += adj;
            _current_image.shader->SetUniform1f("u_gbrightness", _screen.g_brightness);

            return _screen.g_brightness;
        }
        inline float AdjustBrightnessb(float adj)
        {
            _screen.b_brightness += adj;
            _current_image.shader->SetUniform1f("u_bbrightness", _screen.b_brightness);

            return _screen.b_brightness;
        }

        /***************** Contrast Funcs *************************************/

        inline float AdjustContrast(float adj)
        {
            _screen.contrast += adj;
            _current_image.shader->SetUniform1f("u_contrast", _screen.contrast);

            return _screen.contrast;
        }
        inline float AdjustContrastr(float adj)
        {
            _screen.r_contrast += adj;
            _current_image.shader->SetUniform1f("u_rcontrast", _screen.r_contrast);

            return _screen.r_contrast;
        }
        inline float AdjustContrastg(float adj)
        {
            _screen.g_contrast += adj;
            _current_image.shader->SetUniform1f("u_gcontrast", _screen.g_contrast);

            return _screen.g_contrast;
        }
        inline float AdjustContrastb(float adj)
        {
            _screen.b_contrast += adj;
            _current_image.shader->SetUniform1f("u_bcontrast", _screen.b_contrast);

            return _screen.b_contrast;
        }

        /***************** Gamma Funcs *************************************/

        inline float AdjustGamma(float adj)
        {
            _screen.gamma += adj;
            _current_image.shader->SetUniform1f("u_gamma", _screen.gamma);

            return _screen.gamma;
        }
        inline float AdjustGammar(float adj)
        {
            _screen.r_gamma += adj;
            _current_image.shader->SetUniform1f("u_rgamma", _screen.r_gamma);

            return _screen.r_gamma;
        }
        inline float AdjustGammag(float adj)
        {
            _screen.g_gamma += adj;
            _current_image.shader->SetUniform1f("u_ggamma", _screen.g_gamma);

            return _screen.g_gamma;
        }
        inline float AdjustGammab(float adj)
        {
            _screen.b_gamma += adj;
            _current_image.shader->SetUniform1f("u_bgamma", _screen.b_gamma);

            return _screen.b_gamma;
        }

    private:
        Image _current_image;
        float _framerate{0.0f};
        Renderer _renderer;
        AllScreenArray _wall;
        std::string _default_image_location;
        Screen _screen;
        std::unique_ptr<Window>
            _window;
        PlayState _currentState;

    public:
        std::atomic_flag preppingVideo = ATOMIC_FLAG_INIT;
    };
}
