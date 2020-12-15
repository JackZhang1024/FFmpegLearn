//
// Created by 张凤洲 on 2020/12/15.
//

#ifndef FFMPEGLEARN_LKFFMPEG_H
#define FFMPEGLEARN_LKFFMPEG_H

// 如果遇到导入的问题 可以在CMakeLists.txt中的addLibrary中可以添加相应的cpp文件


#include "LKJNICall.h"
#include "LKConstDefine.h"

extern "C" {
   #include "libavformat/avformat.h"
   #include "libswresample/swresample.h"
};

class LKFFmpeg {

public:
    AVFormatContext *pFormatContext = NULL;
    AVCodecContext *pCodecContext = NULL;
    SwrContext *swrContext = NULL;
    uint8_t *resampleOutBuffer = NULL;
    const char* url;
    LKJNICall *pJniCall = NULL;
public:
    LKFFmpeg(LKJNICall *pJniCall, const char* url);
    ~LKFFmpeg();

public:
    void play();

    void callPlayerJniError(int code, char* msg);

    void release();


};


#endif //FFMPEGLEARN_LKFFMPEG_H
