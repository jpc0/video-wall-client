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
namespace Video
{
    enum EventType
    {
        None = 0,
        PlayVideo
    };
    
    class PlayVideoEvent
    {

    };
    union Event
    {
        EventType type;
        PlayVideoEvent playvideo;
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
        dkml::blocking_queue<Event> EventQueue;
    };

    class VideoPlayback{
    public:
        void PlaybackVideo(const std::string &VideoLocation); 
        dkml::blocking_queue<Display::VideoFrame*> frame_queue;
    };
}

#endif