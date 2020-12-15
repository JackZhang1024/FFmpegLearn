#include <jni.h>
#include <string>
#include "LKConstDefine.h"
#include "LKJNICall.h"
#include "LKFFmpeg.h"

// 下面加上这段代码很重要 否则就会报找不到对应的方法引用 unreferenced error
// 在C++中采用c的这种编译方式
extern "C" {
// ffmpeg库
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}


//#define TAG "NDK_LEARN"
//#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
//#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
//#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
//#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
//#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

int add(int a, int b);

extern "C" JNIEXPORT jstring JNICALL
Java_com_luckyboy_ffmpeglearn_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    int c = add(10, 20);
    LOGI("hello world");
    LOGE("result is %d", c);
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

LKJNICall *pJniCall;
LKFFmpeg *pFFmpeg;

jobject initCreateAudioTrack(JNIEnv *env){
    //    public AudioTrack(int streamType, int sampleRateInHz, int channelConfig, int audioFormat,
    //    int bufferSizeInBytes, int mode)
    int streamType = STREAM_TYPE;
    int sampleRateInHz = AUDIO_SAMPLE_RATE;
    int channelConfig = (0x4 | 0x8);
    int audioFormat = 2; // 2个字节
    int bufferSizeInBytes = 0;
    int mode = 1; // 短促的是0 长点的音频是1

    // Mid 方法的ID getMinBufferSizeMid 获取缓冲区
    // static public int getMinBufferSize(int sampleRateInHz, int channelConfig, int audioFormat) {


    jclass jAudioTrackClass = env->FindClass("android/media/AudioTrack");
    jmethodID  jAudioTrackCMid = env->GetMethodID(jAudioTrackClass, "<init>", "(IIIIII)V");

    jmethodID getMinBufferSizeMid = env->GetStaticMethodID(jAudioTrackClass, "getMinBufferSize", "(III)I");
    bufferSizeInBytes = env->CallStaticIntMethod(jAudioTrackClass, getMinBufferSizeMid, sampleRateInHz, channelConfig, audioFormat);


    jobject jAudioTrackObj = env->NewObject(jAudioTrackClass, jAudioTrackCMid, streamType, sampleRateInHz,
            channelConfig, audioFormat, bufferSizeInBytes, mode);

    // play
    jmethodID  playMid = env->GetMethodID(jAudioTrackClass, "play", "()V");
    env->CallVoidMethod(jAudioTrackObj, playMid);

    return jAudioTrackObj;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_luckyboy_ffmpeglearn_FFmpegPlayer_playMedia(JNIEnv *env, jobject jPlayer, jstring url_) {
    pJniCall = new LKJNICall(NULL, env, jPlayer);
    const char *url = env->GetStringUTFChars(url_, 0);
    pFFmpeg = new LKFFmpeg(pJniCall, url);
    pFFmpeg->play();

    delete  pJniCall;
    delete  pFFmpeg;
    env->ReleaseStringUTFChars(url_, url);
    LOGI("Call finish........");
}

/**
 * //    const char *url = env->GetStringUTFChars(url_, 0);
//    LOGI("url:%s", url);
//    av_register_all();
//    AVCodec *c_temp = av_codec_next(NULL);
//    while (c_temp!=NULL){
//        switch (c_temp->type){
//            case AVMEDIA_TYPE_VIDEO:
//                LOGI("[Video]:%s", c_temp->name);
//                break;
//            case AVMEDIA_TYPE_AUDIO:
//                LOGI("[Audio]:%s", c_temp->name);
//                break;
//            default:
//                LOGI("[Other]:%s", c_temp->name);
//                break;
//        }
//        c_temp = c_temp->next;
//    }
 *
 * **/















