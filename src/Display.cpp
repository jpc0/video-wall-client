#include "Display.h"

namespace Display
{
    AllScreenArray::AllScreenArray(const Configuration::ConfigData &configuration)
    {
        _total_width = (configuration.width + configuration.l_bezel + configuration.r_bezel) * configuration.h_screens;
        _total_height = (configuration.height + configuration.t_bezel + configuration.b_bezel) * configuration.v_screens;
    }

    float AllScreenArray::GetAspectRatio()
    {
        return (_total_width / _total_height);
    }

    Display::Display(const Configuration::ConfigData &configuration) : _renderer{configuration},
                                                                       _wall{configuration},
                                                                       _default_image_location{configuration.image_location},
                                                                       _currentState{pImage}
    {
        WindowProps props{"Video Wall Client", (uint32_t)configuration.width, (uint32_t)configuration.height};
        _window = std::unique_ptr<Window>(Window::Create(props));
        _screen.width = configuration.width;
        _screen.height = configuration.height;
        _screen.l_bezel = configuration.l_bezel;
        _screen.r_bezel = configuration.r_bezel;
        _screen.t_bezel = configuration.t_bezel;
        _screen.b_bezel = configuration.b_bezel;
        _screen.h_index = configuration.h_index;
        _screen.v_index = configuration.v_index;

        DisplayDefaultImage();
    }

    Display::~Display()
    {
    }

    void Display::GenerateQuad()
    {
        // Calculate the scale factor of the image, this will blow the image up to the size of the full array of screens
        // and will correct for aspect ratio
        float scale_factor{1};
        float total_pixels_ratio = _wall.GetAspectRatio();
        float texture_pixels_ratio{((float)_current_image.texture->GetWidth() / (float)_current_image.texture->GetHeight())};

        if (total_pixels_ratio > texture_pixels_ratio)
            scale_factor = _wall._total_height / (float)_current_image.texture->GetHeight();
        else if (total_pixels_ratio == texture_pixels_ratio)
            scale_factor = _wall._total_height / (float)_current_image.texture->GetHeight();
        else if (total_pixels_ratio < texture_pixels_ratio)
            scale_factor = _wall._total_width / (float)_current_image.texture->GetWidth();

        float x_origin = ((_wall._total_width - (float)_current_image.texture->GetWidth() * (float)scale_factor) / 2);
        float y_origin = ((_wall._total_height - (float)_current_image.texture->GetHeight() * (float)scale_factor) / 2);
        float x_end = ((float)_current_image.texture->GetWidth() * (float)scale_factor) + x_origin;
        float y_end = ((float)_current_image.texture->GetHeight() * (float)scale_factor) + y_origin;

        TexturedQuad quad{
            x_origin,
            y_origin,
            x_end,
            y_end,
        };

        // Set the 4 vertices of the image, each vertex is x, y, (texture_x), (texture_y)
        // We are creating a square that has the image on it, the image co-ords are from 0.0 to 1.0
        // the square's co-ords are from 0.0 to the total_width of the array of screens
        float positions[] = {
            quad.x_origin, quad.y_origin, quad.x_texture_origin, quad.y_texture_origin, // 1
            quad.x_end, quad.y_origin, quad.x_texture_end, quad.y_texture_origin,       // 2
            quad.x_end, quad.y_end, quad.x_texture_end, quad.y_texture_end,             // 3
            quad.x_origin, quad.y_end, quad.x_texture_origin, quad.y_texture_end};      // 4
        uint32_t indices[] = {
            0, 1, 2,
            2, 3, 0};

        _current_image.va = std::make_unique<VertexArray>();
        _current_image.vb = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
        _current_image.layout = std::make_unique<VertexBufferLayout>();
        _current_image.layout->Push<float>(2);
        _current_image.layout->Push<float>(2);
        _current_image.va->AddBuffer(*_current_image.vb, *_current_image.layout);

        _current_image.ib = std::make_unique<IndexBuffer>(indices, 6);
        const std::string vs_source =
#include "../res/shaders/Basic.shader"
            ;
        _current_image.shader = std::make_unique<Shader>(vs_source, true);

        _current_image.shader->Bind();
        _current_image.texture->Bind();

        _current_image.shader->SetUniform1i("u_Texture", 0);

        ProcessColour();

        glm::mat4 mvp{1.0f};
        // proj is the size of the local screen
        glm::mat4 proj = glm::ortho(0.0f, (float)_screen.width, 0.0f, (float)_screen.height, -1.0f, 1.0f);
        // We move the camera to where it should be in relation to the full array of screens
        glm::mat4 view = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f - (float)((_screen.width + (_screen.l_bezel + _screen.r_bezel)) * _screen.h_index), 0.0f - (float)((_screen.height + (_screen.t_bezel + _screen.b_bezel)) * _screen.v_index), 0.0f});

        _current_image.shader->SetUniformMat4f("u_MVP", mvp);
        _current_image.shader->SetUniformMat4f("u_Proj", proj);
        _current_image.shader->SetUniformMat4f("u_View", view);
        _current_image.va->Unbind();
        _current_image.vb->Unbind();
        _current_image.ib->Unbind();
        _current_image.shader->Unbind();
    }

    void Display::DisplaySingleImage(const std::string &image_location)
    {
        _currentState = PlayState::pImage;
        _current_image.texture = std::make_unique<Texture>(image_location);
        GenerateQuad();
    }

    void Display::PlayVideo(Frame &frame, float framerate)
    {
        _framerate = framerate;
        _currentState = PlayState::pVideo;
        _current_image.texture = std::make_unique<Texture>(frame);
        GenerateQuad();
    }

    void Display::UpdateVideoFrame(Frame &frame)
    {
        _current_image.texture = std::make_unique<Texture>(frame);
    }

    void Display::ProcessColour()
    {
        _current_image.shader->SetUniform1f("u_brightness", _screen.brightness);
        _current_image.shader->SetUniform1f("u_rbrightness", _screen.r_brightness);
        _current_image.shader->SetUniform1f("u_gbrightness", _screen.g_brightness);
        _current_image.shader->SetUniform1f("u_bbrightness", _screen.b_brightness);

        _current_image.shader->SetUniform1f("u_contrast", _screen.contrast);
        _current_image.shader->SetUniform1f("u_rcontrast", _screen.r_contrast);
        _current_image.shader->SetUniform1f("u_gcontrast", _screen.g_contrast);
        _current_image.shader->SetUniform1f("u_bcontrast", _screen.b_contrast);

        _current_image.shader->SetUniform1f("u_gamma", _screen.gamma);
        _current_image.shader->SetUniform1f("u_rgamma", _screen.r_gamma);
        _current_image.shader->SetUniform1f("u_ggamma", _screen.g_gamma);
        _current_image.shader->SetUniform1f("u_bgamma", _screen.b_gamma);
    }

    void Display::Refresh()
    {
        _renderer.Clear();
        _renderer.Draw(*_current_image.va, *_current_image.ib, *_current_image.shader);
        _window->OnUpdate();
        glfwPollEvents();
    }
}
