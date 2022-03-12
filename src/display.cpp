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

        DisplayDefaultImage();
    }

    Display::~Display()
    {
    }


    void Display::GenerateQuad()
    {
        float scale_factor{0};
        float total_pixels_ratio = m_wall.GetAspectRatio();
        float texture_pixels_ratio = static_cast<float>(m_current_image.getSize().x)/static_cast<float>(m_current_image.getSize().y);
        if (total_pixels_ratio > texture_pixels_ratio)
            scale_factor =  m_wall._total_height/ static_cast<float>(m_current_image.getSize().y);
        else if (total_pixels_ratio == texture_pixels_ratio)
            scale_factor = m_wall._total_height/ static_cast<float>(m_current_image.getSize().y);
        else if (total_pixels_ratio < texture_pixels_ratio)
            scale_factor = m_wall._total_width/ static_cast<float>(m_current_image.getSize().x);
        
        m_current_image.setSmooth(true);
        m_current_display.setTexture(m_current_image);
        m_current_display.scale({scale_factor,scale_factor});

        m_current_display.move({(m_wall._total_width - m_current_display.getGlobalBounds().width)/2,
                                (m_wall._total_height - m_current_display.getGlobalBounds().height)/2});

        m_current_display.move({0.0f - static_cast<float>(((m_screen.width + (m_screen.l_bezel + m_screen.r_bezel)) * m_screen.h_index)), 
                                0.0f - static_cast<float>(((m_screen.height + (m_screen.t_bezel + m_screen.b_bezel)) * m_screen.v_index))});
    }

    void Display::DisplaySingleImage(const std::string &image_location)
    {
        if (!m_current_image.loadFromFile(image_location))
            exit(EXIT_FAILURE);
        
        GenerateQuad();
    }

    void Display::Refresh(sf::RenderWindow &window)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape)
                        window.close();
                    break;
                default:
                    break;
            }
        }
        window.clear();
        window.draw(m_current_display);
        window.display();
    }
}