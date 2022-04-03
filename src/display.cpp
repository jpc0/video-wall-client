#include "display.hpp"
#include <spdlog/spdlog.h>
namespace Display
{
    void destroyTexture(SDL_Texture *texture)
    {
        SDL_DestroyTexture(texture);
    }

    AllScreenArray::AllScreenArray(const Configuration::ConfigData &configuration)
    {
        _total_width = static_cast<float>((configuration.width + configuration.l_bezel + configuration.r_bezel) * configuration.h_screens);
        _total_height = static_cast<float>((configuration.height + configuration.t_bezel + configuration.b_bezel) * configuration.v_screens);
    }

    float AllScreenArray::GetAspectRatio() const
    {
        return (_total_width / _total_height);
    }

    Display::Display(const Configuration::ConfigData &configuration, [[ maybe_unused ]] const CustomMessages &customMessage) :
                m_current_image{nullptr, &destroyTexture},
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
        m_source_cropping = nullptr;
        m_current_image = nullptr;
        DisplayDefaultImage();
    }

    Display::~Display()
    {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
    }


    void Display::GenerateQuad()
    {

        float scale_factor{0};
        float total_pixels_ratio = m_wall.GetAspectRatio();
        int imgW;
        int imgH;
        SDL_QueryTexture(m_current_image.get(), nullptr, nullptr, &imgW, &imgH);
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
        SDL_Surface *img = IMG_Load(image_location.c_str());
        m_current_image.reset(SDL_CreateTextureFromSurface(m_renderer, img));
        SDL_FreeSurface(img);
        if (!m_current_image)
        {
            spdlog::info("Failed to read image at: {}", image_location);
            exit(EXIT_FAILURE);
        }
        GenerateQuad();
    }

    void Display::Refresh()
    {
        if (m_playingVideo)
            GetFrame();
        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer, m_current_image.get(), m_source_cropping.get(), m_current_display.get());
        SDL_RenderPresent(m_renderer);
    }

    void Display::PrepVideo(VideoType *Video)
    {
        spdlog::debug("Asked to Prepare Video");
        m_current_image.reset(
            SDL_CreateTexture(
                m_renderer, 
                SDL_PIXELFORMAT_IYUV, //FFMPEG sends us IYUV data not YV12
                SDL_TEXTUREACCESS_STREAMING, 
                Video->Width, 
                Video->Height
                )
            );
        m_VideoFrameQueue = Video->Queue;
        m_playingVideo = true;
        delete Video;
        GenerateQuad();
    }

    void Display::GetFrame()
    {
        spdlog::debug("Getting Video frame and displaying it");
        auto pFrame = *m_VideoFrameQueue->dequeue();
        assert(pFrame != nullptr);
        spdlog::debug("Reading frame number: {}", pFrame->framenumber);
        if (pFrame->ended)
        {
            m_playingVideo = false;
            m_VideoFrameQueue.reset();
            DisplayDefaultImage();
        }
        else
        {
            auto data = *pFrame->data.get();

            // SDL will be passing us a pointer to the data that we will
            // memcpy our data into
            void *pixels = nullptr;
            int linesize = 0;
            SDL_LockTexture(
                m_current_image.get(),
                nullptr,
                &pixels,
                &linesize
            );
            // We already ensured about getting the correct memory layout before
            // we sent the vector here
            SDL_memcpy(
                pixels,
                data.data(),
                data.size()
            );
            SDL_UnlockTexture(m_current_image.get());
        }
    }
}