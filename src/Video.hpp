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

namespace Video
{
    class Video
    {
    public:
        Video();
    };
}

#endif