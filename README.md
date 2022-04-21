## Network API

The network API will use ZeroMQ as a communication's protocol, this will allow
us to do more things than a REST API would allow, it will expect the follow
JSON encoded data
```
{
    "version": "0.1.0",
    "command": "display_image",
    "location": "http://fileserve.example.com/${guid}.jpeg", // Optional depending on command
}
```

## Build - Required Packages

Please make sure the following packages are installed on your system:
- FFMPEG
  - avcodec-dev
  - avformat-dev
  - avfilter-dev
  - swscale-dev
  - avutil-dev
- SDL2
  - sdl2-dev
- SDL2_image
  - sdl2_image-dev

Reasonable recent source files for these packages are included in the vendor
folder if you need to build them from source however it is strongly recommended
that you use the package manager bundled with your distro on Linux, the 
official binaries for windows or homebrew for Mac.