#include "Video.hpp"
#include <iostream>
#include <utility>
#include "display.hpp"
extern "C"{
    #include <libswscale/swscale.h>
}
namespace Video
{
    void freeFrame(AVFrame *pFrame)
    {
        spdlog::debug("Freeing frame: {}", static_cast<void *>(pFrame));
        av_frame_free(&pFrame);
    }

    void freeFormatContext(AVFormatContext *pFormatContext)
    {
        spdlog::debug("Freeing Format Context: {}", static_cast<void *>(pFormatContext));
        avformat_close_input(&pFormatContext);
    }

    void freeCodecContext(AVCodecContext *pCodecContext)
    {
        spdlog::debug("Freeing Codec Context: {}", static_cast<void *>(pCodecContext));
        avcodec_free_context(&pCodecContext);
    }

    void freePacket(AVPacket *pPacket)
    {
        spdlog::debug("Freeing Packet: {}", static_cast<void *>(pPacket));
        av_packet_free(&pPacket);
    }

    Video::Video(std::string VideoLocation, std::shared_ptr<WnLSL::blocking_rb_queue<std::shared_ptr<Display::VideoFrame>>> FrameQueue)
        :
        m_VideoLocation{std::move(VideoLocation)},
        VideoQueue{MessageHandler::registerReceiver(Destination::VideoMessage)},
        frame_queue{std::move(FrameQueue)}
    {
    }

    void Video::PlaybackVideo()
    {
        std::unique_ptr<AVFormatContext, decltype(&freeFormatContext)> FormatContext(avformat_alloc_context(), &freeFormatContext);
        AVFormatContext *pFormatContext = FormatContext.get();
        if (avformat_open_input(&pFormatContext, m_VideoLocation.c_str(), nullptr, nullptr) != 0)
        {
            spdlog::info("Failed to open file");
            exit(EXIT_FAILURE);
        }
        avformat_find_stream_info(pFormatContext, nullptr);

        const AVCodec *pCodec = nullptr;
        const AVCodecParameters *pCodecParams = nullptr;
        int video_stream_index = -1;

        for (auto i = 0; i < static_cast<int>(pFormatContext->nb_streams); ++i)
        {
            const AVCodecParameters *pLocalCodecParams = pFormatContext->streams[i]->codecpar;
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

        nlohmann::json videoinfo
        {
          { "PixelFormat", Display::PixelFormat::YUV },
          { "Width", pCodecParams->width },
          { "Height", pCodecParams->height },
        };
        VideoQueue->send({
          Destination::DisplayMessage,
          {
            {"Command", "VideoInfo"},
            {"Data", videoinfo}
          }
        });

        std::unique_ptr<AVCodecContext, decltype(&freeCodecContext)> pCodecContext(avcodec_alloc_context3(pCodec), &freeCodecContext);
        avcodec_parameters_to_context(pCodecContext.get(), pCodecParams);
        avcodec_open2(pCodecContext.get(), pCodec, nullptr);
        std::unique_ptr<AVPacket, decltype(&freePacket)> pPacket(av_packet_alloc(), &freePacket);
        while (av_read_frame(pFormatContext, pPacket.get()) >= 0)
        {
            if (pPacket->stream_index == video_stream_index){
                int res = avcodec_send_packet(pCodecContext.get(), pPacket.get());
                while (res >= 0)
                {
                    std::unique_ptr<AVFrame, decltype(&freeFrame)> pFrame(av_frame_alloc(), &freeFrame);
                    if (pFrame == nullptr)
                        continue;
                    res = avcodec_receive_frame(pCodecContext.get(), pFrame.get());
                    if (pFrame->width > 0 && pFrame->height > 0)
                    {
                        auto data = std::make_shared<std::vector<uint8_t>>();
                        // This is the Y channel
                        data->insert(
                            data->end(),
                            &pFrame->data[0][0],
                            // The entire array is linesize * height bytes large
                            // because the array contains bytes this very nicely
                            // means that the array is also linesize*height long
                            &pFrame->data[0][(pFrame->linesize[0]*(pFrame->height))-1] // -1 becauuse arrays count from 0
                            );
                        // FFMPEG pads the end of the UV channels with 0s, SDL expects
                        // the input without these so we just strip them
                        data->insert(
                            data->end(),
                            &pFrame->data[1][0],
                            // The second half of the array is just padded with 0s
                            &pFrame->data[1][((pFrame->linesize[1]/2)*(pFrame->height))-1] // -1 because arrays count from 0
                            );
                        data->insert(
                            data->end(),
                            &pFrame->data[2][0],
                            // The second half of the array is just padded with 0s
                            &pFrame->data[2][((pFrame->linesize[2]/2)*(pFrame->height))-1] // -1 because arrays count from 0
                            );
                        Display::VideoFrame frame_{
                            data,
                            pFrame->width,
                            pFrame->height,
                            false,
                            static_cast<int>(pFrame->best_effort_timestamp)
                        };
                        frame_queue->enqueue(std::make_shared<Display::VideoFrame>(frame_));
                    }
                }
            }
            av_packet_unref(pPacket.get());

        }
        Display::VideoFrame frame_{
            nullptr,
            0,
            0,
            true,
            -1
        };
        frame_queue->enqueue(std::make_shared<Display::VideoFrame>(frame_));
    }


}
