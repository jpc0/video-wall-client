#include "Video.hpp"
#include <iostream>
#include "display.hpp"
#include <type_traits>
namespace Video
{
    static void freeFrame(Display::VideoFrame *Frame)
    {
        AVFrame *pFrame = std::bit_cast<AVFrame *>(Frame->underlying_frametypeptr);
        spdlog::info("Freeing frame: {}", pFrame->best_effort_timestamp);
        av_frame_free(&pFrame);
        delete Frame;
    }

    Video::Video(){
        VideoPlayback currentvideo{};
        currentvideo.PlaybackVideo("/home/jpc0/Downloads/bigbun.mp4");
    }

    void VideoPlayback::PlaybackVideo(const std::string &VideoLocation)
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
        AVFormatContext *pFormatContext = avformat_alloc_context();

        if (avformat_open_input(&pFormatContext, VideoLocation.c_str(), nullptr, nullptr) != 0)
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
        Display::VideoType *videoinfo = new Display::VideoType;
        videoinfo->format =  Display::PixelFormat::YUV;
        videoinfo->Height = pCodecParams->height;
        videoinfo->Width = pCodecParams->width;
        delete videoinfo;
        AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
        avcodec_parameters_to_context(pCodecContext, pCodecParams);
        avcodec_open2(pCodecContext, pCodec, nullptr);
        AVPacket *pPacket = av_packet_alloc();
        while (av_read_frame(pFormatContext, pPacket) >= 0)
        {
            if (pPacket->stream_index == video_stream_index){
                int res = avcodec_send_packet(pCodecContext, pPacket);
                while (res >= 0)
                {
                    AVFrame *pFrame = av_frame_alloc();
                    res = avcodec_receive_frame(pCodecContext, pFrame);
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
                        Display::VideoFrame *frame = new Display::VideoFrame;
                        frame->data = pFrame->data;
                        frame->line_length = pFrame->linesize;
                        frame->underlying_frametypeptr = pFrame;

                        frame_queue.push(frame);
                        break;
                        // TODO: After the first instance of doing this we will
                        // end up passing an event to run the Display::BeginVideo
                        // function
                    }
                    av_frame_free(&pFrame);
                }
            }
            av_packet_unref(pPacket);

        }
        frame_queue.push(nullptr);
        avformat_close_input(&pFormatContext);
        av_packet_free(&pPacket);
        avcodec_free_context(&pCodecContext);
        
        // Test code to confirm that we can in fact access the queue, this will
        // not actually exist and we will be calling freeFrame from an event that
        // get's passed to us 
        while (true)
        {
            Display::VideoFrame *pFrame = frame_queue.pop();
            if (pFrame == 0)
            {
                spdlog::info("End of video reached");
                break;
            }
            freeFrame(pFrame);
        }

    }


}
