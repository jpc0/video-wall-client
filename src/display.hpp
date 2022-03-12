#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "Configuration.hpp"

namespace Display
{
    struct Screen
    {
        int width;
        int height;
        int l_bezel;
        int r_bezel;
        int t_bezel;
        int b_bezel;
        int h_index;
        int v_index;
    };

    class AllScreenArray
    {
    public:
        explicit AllScreenArray(const Configuration::ConfigData &configuration);
        ~AllScreenArray() = default;
        float GetAspectRatio() const;

    public:
        float _total_width;
        float _total_height;
    };

    class Display
    {
    public:
        explicit Display(const Configuration::ConfigData &configuration);
        ~Display();
        void DisplaySingleImage(const std::string &image_location);
        void GenerateQuad();
        void Refresh(sf::RenderWindow &window);

        inline void DisplayDefaultImage() { DisplaySingleImage(m_default_image_location); }
    private:
        std::string m_current_image_location;
        sf::Texture m_current_image;
        sf::Sprite m_current_display;
        AllScreenArray m_wall;
        std::string m_default_image_location;
        Screen m_screen;
    }; 
} // namespace Display
