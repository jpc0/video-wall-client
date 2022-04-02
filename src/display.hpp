#pragma once

#include <string>
#include <map>
#include <vector>

#include "Configuration.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "WnLSL/queues/ringbuffer_queue.hpp"
#include "main.hpp"

namespace Display
{
    void destroyTexture(SDL_Texture *texture);

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
        std::shared_ptr<std::vector<uint8_t>> data;
        int width;
        int height;
        bool ended = false;
        int framenumber;
    };

    enum PixelFormat
    {
        Unknown = 0,
        YUV = 1,
        RGB24 = 2

    }; 
    struct VideoType
    {
        PixelFormat format;
        int Width;
        int Height;
        std::shared_ptr<WnLSL::blocking_rb_queue<std::shared_ptr<VideoFrame>>> Queue;
    };

    static std::map<PixelFormat, SDL_PixelFormatEnum> const PixelFormatMapping{
        {PixelFormat::Unknown, SDL_PIXELFORMAT_UNKNOWN},
        {PixelFormat::YUV, SDL_PIXELFORMAT_IYUV},
        {PixelFormat::RGB24, SDL_PIXELFORMAT_RGB24}
    };

    class Display
    {
    public:
        explicit Display(const Configuration::ConfigData &configuration, const CustomMessages &customMessage);
        ~Display();
        void DisplaySingleImage(const std::string &image_location);
        void PrepVideo(VideoType *Video);
        void BeginVideo();
        void GenerateQuad();
        void Refresh();
        void GetFrame();
        inline void DisplayDefaultImage() { DisplaySingleImage(m_default_image_location); }
    private:
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        std::string m_current_image_location;
        std::unique_ptr<SDL_Texture, decltype(&destroyTexture)> m_current_image;
        std::unique_ptr<SDL_Rect> m_source_cropping;
        std::unique_ptr<SDL_Rect> m_current_display;
        AllScreenArray m_wall;
        std::string m_default_image_location;
        Screen m_screen;
        bool m_playingVideo = false;
        std::shared_ptr<WnLSL::blocking_rb_queue<std::shared_ptr<VideoFrame>>> m_VideoFrameQueue =  nullptr;
    }; 
} // namespace Display
