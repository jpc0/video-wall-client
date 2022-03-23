#include "Video.hpp"

namespace Video
{
    Video::Video(){

        // This is purely test code, it will definitely need to be moved to a different
        // member function, we will be pushing the frames into a queue that will be
        // read by the display class to display the videos with the correct time
        // base and concidering VSYNC, all of this code will likely happen in it's own thread
        AVFormatContext *pFormatContext = avformat_alloc_context();

        if (avformat_open_input(&pFormatContext, "/home/jpc0/Downloads/bigbun.mp4", nullptr, nullptr) != 0)
        {
            spdlog::info("Failed to open file");
            exit(EXIT_FAILURE);
        }
        spdlog::info("Format {}, duration {} us", pFormatContext->iformat->long_name, pFormatContext->duration);

        avformat_find_stream_info(pFormatContext, nullptr);

        const AVCodec *pCodec = nullptr;

        const AVCodecParameters *pCodecParams = nullptr;

        int video_stream_index = -1;

        for (auto i = 0; i < static_cast<int>(pFormatContext->nb_streams); ++i)
        {
            const AVCodecParameters *pLocalCodecParams = nullptr;
            pLocalCodecParams = pFormatContext->streams[i]->codecpar;
            const AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParams->codec_id);

            if (pLocalCodecParams->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                if (video_stream_index == -1)
                {
                    video_stream_index = i;
                    pCodec = pLocalCodec;
                    pCodecParams = pLocalCodecParams;
                }
            }    
        }
        if(pCodecParams == nullptr || pCodec == nullptr)
            exit(EXIT_FAILURE); // We have no context or codec information
        AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
        avcodec_parameters_to_context(pCodecContext, pCodecParams);
        avcodec_open2(pCodecContext, pCodec, nullptr);
        AVFrame *pFrame = av_frame_alloc();
        AVPacket *pPacket = av_packet_alloc();

        while (av_read_frame(pFormatContext, pPacket) >= 0)
        {
            if (pPacket->stream_index == video_stream_index)
            {
                avcodec_receive_frame(pCodecContext, pFrame);
                // It is important that we create a texture with the same
                // pixel format in Display as what we have here, something to
                // keep in mind is that we will probably need a translation map
                // from FFmpeg's pixel format enum to SDLs
                // 
                // There is information under pFrame->crop_* that we can use
                // to create an SDL_Rect for writing which is the information
                // we do not care about in pFrame->data
                spdlog::info("Pixel format is: {}", pCodecContext->pix_fmt);
            }
            av_packet_unref(pPacket);
        }

        avformat_close_input(&pFormatContext);
        av_packet_free(&pPacket);
        av_frame_free(&pFrame);
        avcodec_free_context(&pCodecContext);
}
}