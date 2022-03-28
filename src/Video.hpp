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
#include "Util.hpp"
#include "display.hpp"
#include <thread>

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
        explicit VideoPlayback(const char* VideoLocation);
        void PlaybackVideo(); 
        dkml::blocking_queue<Display::VideoFrame> frame_queue;
    private:
        std::string m_VideoLocation;
        std::jthread playbackThread;
    };
    class Video
    // We should create a video class on startup that then blocks while waiting
    // for a play_video event, it is going to be rather important that we in fact
    // build up this event queue ourselves, we can likely just use our blocking
    // queue that we have define already, we should probably make that queue 
    // generic over types
    {
    public:
        Video();
        void GetVideo();
        VideoPlayback currentvideo;  
        dkml::blocking_queue<Event> EventQueue;
        bool videoend = false;
    };

}

#endif