#pragma once
#include <cstdint>

struct Frame
{
    int width;
    int height;
    int bpp;
    uint8_t *buffer;
};