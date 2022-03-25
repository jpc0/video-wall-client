#pragma once

#include <string>
#include <map>

#include "Configuration.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

    struct VideoFrame
    {
        int width;
        int height;
        uint8_t *data;
        int *line_length;
        int pixel_format;
    };

    std::map<int, int> const pixel_format_mapping{
        { -1 , SDL_PIXELFORMAT_UNKNOWN},
        {0, SDL_PIXELFORMAT_IYUV},
        {2, SDL_PIXELFORMAT_RGB24} 
    };
    
    class Display
    {
    public:
        explicit Display(const Configuration::ConfigData &configuration);
        ~Display();
        void DisplaySingleImage(const std::string &image_location);
        void PrepVideo(int pixel_format, int width, int height);
        void BeginVideo();
        void GenerateQuad();
        void Refresh();

        inline void DisplayDefaultImage() { DisplaySingleImage(m_default_image_location); }
    private:
        SDL_Window *m_window = nullptr;
        SDL_Renderer *m_renderer = nullptr;
        std::string m_current_image_location;
        SDL_Texture *m_current_image = nullptr;
        std::unique_ptr<SDL_Rect> m_source_cropping = nullptr;
        std::unique_ptr<SDL_Rect> m_current_display = nullptr;
        AllScreenArray m_wall;
        std::string m_default_image_location;
        Screen m_screen;
    }; 
} // namespace Display
