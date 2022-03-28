#include "Video.hpp"
#include <iostream>
#include "display.hpp"
#include <type_traits>
namespace Video
{
    void freeFrame(AVFrame *pFrame)
    {
        spdlog::info("Freeing frame: {}", static_cast<void *>(pFrame));
        av_frame_free(&pFrame);
    }

    void freeFormatContext(AVFormatContext *pFormatContext)
    {
        spdlog::info("Freeing Format Context: {}", static_cast<void *>(pFormatContext));
        avformat_close_input(&pFormatContext);
    }

    void freeCodecContext(AVCodecContext *pCodecContext)
    {
        spdlog::info("Freeing Codec Context: {}", static_cast<void *>(pCodecContext));
        avcodec_free_context(&pCodecContext);
    }

    void freePacket(AVPacket *pPacket)
    {
        spdlog::info("Freeing Packet: {}", static_cast<void *>(pPacket));
        av_packet_free(&pPacket);
    }
    Video::Video()
        // This won't be initialized here but will rather be assigned from an 
        // event that will be sent to us
        : currentvideo{"/home/jpc0/Downloads/bigbun.mp4"}
    {
    }

    void Video::GetVideo()
        // Test code to confirm that we can in fact access the queue, this will
        // not actually exist and we will be calling freeFrame from an event that
        // get's passed to us 
    {
        if (!videoend)
        {
        Display::VideoFrame pFrame = currentvideo.frame_queue.pop();
        if (pFrame.underlying_frametypeptr == 0)
        {
            spdlog::info("End of video reached");
            videoend = true;
            return;
        }
        freeFrame(static_cast<AVFrame*>(pFrame.underlying_frametypeptr));
        }
    }

    VideoPlayback::VideoPlayback(const char* VideoLocation)
        : m_VideoLocation{VideoLocation} 
        , playbackThread(&VideoPlayback::PlaybackVideo, this)
    {
    }

    void VideoPlayback::PlaybackVideo()
    {
        // This is purely test code, it will definitely need to be moved to a different
        // member function, we will be pushing the frames into a queue that will be
        // read by the display class to display the videos with the correct time
        // base and concidering VSYNC, all of this code will likely happen in it's own thread
        // The idea here is that we will go all the way to before we start generating
        // frames in the main thread. We will then spawn a thread to generate all the frames
        // and stick them into a queue.
        // We will send an event with a pointer to the queue to signal to display to
        // start displaying the video the moment we have encoded a single frame
        // hopefully we don't have memory issues here, otherwise we will probably
        // just restrict the queue size and block once the queue is full 
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

        // TODO: We have enough information here to pass to the display server
        // to generate the information for when we start generating frames
        // this is where we will pass an event to end up calling 
        // Dispay::PrepVideo

        // We need to create a mapping from ffmpegs pixel format enum to 
        // Display's pixel format enum
        spdlog::info("Codec video format is: {}", pCodecParams->format);
        spdlog::info("Codec video width is: {}", pCodecParams->width);
        spdlog::info("Codec video height is: {}", pCodecParams->height);

        [[ maybe_unused ]] Display::VideoType videoinfo{
        Display::PixelFormat::YUV,
        pCodecParams->height,
        pCodecParams->width,
        &frame_queue
        };
        
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
                    res = avcodec_receive_frame(pCodecContext.get(), pFrame.get());
                    if (pFrame->width > 0)
                    {
                        // It is important that we create a texture with the same
                        // pixel format in Display as what we have here, something to
                        // keep in mind is that we will probably need a translation map
                        // from FFmpeg's pixel format enum to SDLs
                        // 
                        // There is information under pFrame->crop_* that we can use
                        // to create an SDL_Rect for writing which is the information
                        // we do not care about in pFrame->data
                        // spdlog::info("Pixel format is: {}", pFrame->format);
                        // spdlog::info("Top crop is: {}", pFrame->crop_top);
                        // spdlog::info("Bottom crop is: {}", pFrame->crop_bottom);
                        // spdlog::info("Left crop is: {}", pFrame->crop_left);
                        // spdlog::info("Right crop is: {}", pFrame->crop_right);
                        // spdlog::info("Picute type is: {}",av_get_picture_type_char(pFrame->pict_type));
                        // spdlog::info("Width is: {}", pFrame->width);
                        // spdlog::info("Height is: {}", pFrame->height);
                        // spdlog::info("Packet size is: {}", pFrame->pkt_size);
                        // spdlog::info("Line length is: {}", pFrame->linesize[0]);
                        // spdlog::info("Frame data: {}", *pFrame->data);
                        Display::VideoFrame frame{};
                        frame.data = pFrame->data;
                        frame.line_length = pFrame->linesize;
                        frame.underlying_frametypeptr = pFrame.release();

                        spdlog::info("Sending frame: {}", frame.underlying_frametypeptr);
                        frame_queue.push(frame);
                        break;
                        // TODO: After the first instance of doing this we will
                        // end up passing an event to run the Display::BeginVideo
                        // function
                    }
                }
            }
            av_packet_unref(pPacket.get());

        }
        Display::VideoFrame frame{};
        frame.data = nullptr;
        frame.line_length = nullptr;
        frame.underlying_frametypeptr = nullptr;
        frame_queue.push(frame);
    }


}
