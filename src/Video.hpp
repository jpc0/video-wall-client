#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <spdlog/spdlog.h>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include "display.hpp"
#include <thread>
#include "WnLSL/queues/ringbuffer_queue.hpp"
#include "main.hpp"

namespace Video
{
    void freeFrame(AVFrame *pFrame);
    void freeFormatContext(AVFormatContext *pFormatContext);
    void freeCodecContext(AVCodecContext *pCodecContext);
    void freePacket(AVPacket *pPacket);

    enum EventType
    {
        None = 0,
        PlayVideo,
        FreeFrame
    };
    
    struct PlayVideoEvent
    {
        int Type = EventType::PlayVideo;
        const char* VideoLocation;
    };

    struct freeFrameEvent
    {
        int Type = EventType::FreeFrame;
        void *frameptr;
    };

    union Event
    {
        EventType type;
        PlayVideoEvent playvideo;
        freeFrameEvent freeframe;
    };
   
    class VideoPlayback{
    public:
        explicit VideoPlayback(const char* VideoLocation, uint32_t videoReadyEvent);
        void PlaybackVideo(); 
        std::shared_ptr<WnLSL::blocking_rb_queue<std::shared_ptr<Display::VideoFrame>>> frame_queue = 
            std::make_shared<WnLSL::blocking_rb_queue<std::shared_ptr<Display::VideoFrame>>>();
    private:
        std::string m_VideoLocation;
        uint32_t videoReady;
    };
    class Video
    {
    private:
        const uint32_t videoReady;
    public:
        explicit Video(const CustomMessages &customMessages);
        WnLSL::blocking_rb_queue<Event> EventQueue;
    };

}

#endif