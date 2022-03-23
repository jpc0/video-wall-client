#include "display.hpp"
#include <spdlog/spdlog.h>
namespace Display
{
    AllScreenArray::AllScreenArray(const Configuration::ConfigData &configuration)
    {
        _total_width = static_cast<float>((configuration.width + configuration.l_bezel + configuration.r_bezel) * configuration.h_screens);
        _total_height = static_cast<float>((configuration.height + configuration.t_bezel + configuration.b_bezel) * configuration.v_screens);
    }

    float AllScreenArray::GetAspectRatio() const
    {
        return (_total_width / _total_height);
    }

     Display::Display(const Configuration::ConfigData &configuration) :
                m_wall{configuration},
                m_default_image_location{configuration.image_location}
    {
        m_screen.width = configuration.width;
        m_screen.height = configuration.height;
        m_screen.l_bezel = configuration.l_bezel;
        m_screen.r_bezel = configuration.r_bezel;
        m_screen.t_bezel = configuration.t_bezel;
        m_screen.b_bezel = configuration.b_bezel;
        m_screen.h_index = configuration.h_index;
        m_screen.v_index = configuration.v_index;

        m_window = SDL_CreateWindow(
            "Video Wall Client", 
            SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), 
            SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), 
            static_cast<int>(configuration.width), 
            static_cast<int>(configuration.height), SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);
        SDL_ShowCursor(SDL_DISABLE);
        DisplayDefaultImage();
    }

    Display::~Display()
    {
        SDL_DestroyTexture(m_current_image);
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
    }


    void Display::GenerateQuad()
    {

        float scale_factor{0};
        float total_pixels_ratio = m_wall.GetAspectRatio();
        int imgW;
        int imgH;
        SDL_QueryTexture(m_current_image, nullptr, nullptr, &imgW, &imgH);
        float texture_pixels_ratio = static_cast<float>(imgW)/static_cast<float>(imgH);
        if (total_pixels_ratio > texture_pixels_ratio)
            scale_factor =  m_wall._total_height/ static_cast<float>(imgH);
        else if (total_pixels_ratio == texture_pixels_ratio)
            scale_factor = m_wall._total_height/ static_cast<float>(imgH);
        else if (total_pixels_ratio < texture_pixels_ratio)
            scale_factor = m_wall._total_width/ static_cast<float>(imgW);

        if (m_current_display == nullptr)
            m_current_display = std::make_unique<SDL_Rect>();
        m_current_display->w = static_cast<int>(static_cast<float>(imgW) * scale_factor);
        m_current_display->h = static_cast<int>(static_cast<float>(imgH) * scale_factor);


        m_current_display->x = static_cast<int>(0.0f - static_cast<float>(((m_screen.width + (m_screen.l_bezel + m_screen.r_bezel)) * m_screen.h_index)));
        m_current_display->y = static_cast<int>(0.0f - static_cast<float>(((m_screen.height + (m_screen.t_bezel + m_screen.b_bezel)) * m_screen.v_index)));
        m_current_display->x += static_cast<int>((m_wall._total_width - static_cast<float>(m_current_display->w))/2.0f);
        m_current_display->y += static_cast<int>((m_wall._total_height - static_cast<float>(m_current_display->h))/2.0f);
    }

    void Display::DisplaySingleImage(const std::string &image_location)
    {
        if (m_current_image)
            SDL_DestroyTexture(m_current_image);
        m_current_image = nullptr;
        SDL_Surface *img = IMG_Load(image_location.c_str()); 
        m_current_image = SDL_CreateTextureFromSurface(m_renderer, img);
        SDL_FreeSurface(img);
        if (!m_current_image)
            exit(EXIT_FAILURE);
        
        GenerateQuad();
    }

    void Display::Refresh()
    {
        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer, m_current_image, m_source_cropping.get(), m_current_display.get());
        SDL_RenderPresent(m_renderer);
    }
}