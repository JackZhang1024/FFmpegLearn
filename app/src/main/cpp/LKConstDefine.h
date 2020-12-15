//
// Created by 张凤洲 on 2020/12/10.
//

#ifndef FFMPEGLEARN_LKCONSTDEFINE_H
#define FFMPEGLEARN_LKCONSTDEFINE_H

// 打印日志
#include <android/log.h>

#define TAG "NDK_LEARN"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


#define AUDIO_SAMPLE_RATE  44100
#define STREAM_TYPE 3

// ------------播放错误码 start---------------
#define FIND_STREAM_ERROR_CODE -0X10
#define CODEC_FIND_DECODER_ERROR_CODE -0X11
#define CODEC_ALLOC_CONTEXT_ERROR_CODE -0x12
#define SWR_ALLOC_SET_OPTS_ERROR_CODE -0x13
#define SWR_INIT_ERROR_CODE -0x14


// ------------播放错误码 end---------------



#endif //FFMPEGLEARN_LKCONSTDEFINE_H
