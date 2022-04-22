#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <spdlog/spdlog.h>
#include "MessageHandler/MessageHandler.hpp"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <thread>
#include "WnLSL/queues/ringbuffer_queue.hpp"
#include "main.hpp"
#include <nlohmann/json.hpp>

namespace Display
{
  class VideoFrame;
}

namespace Video
{
    void freeFrame(AVFrame *pFrame);
    void freeFormatContext(AVFormatContext *pFormatContext);
    void freeCodecContext(AVCodecContext *pCodecContext);
    void freePacket(AVPacket *pPacket);

    class Video
    {
    public:
        Video(std::string VideoLocation, std::shared_ptr<WnLSL::blocking_rb_queue<std::shared_ptr<Display::VideoFrame>>> FrameQueue);
        void PlaybackVideo(); 
    private:
        std::string m_VideoLocation;
        std::shared_ptr<MessageQueue> VideoQueue;
        std::shared_ptr<WnLSL::blocking_rb_queue<std::shared_ptr<Display::VideoFrame>>> frame_queue;
    };
}

#endif