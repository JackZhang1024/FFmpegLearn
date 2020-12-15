//
// Created by 张凤洲 on 2020/12/13.
//

#include "LKJNICall.h"
#include "LKConstDefine.h"

LKJNICall::LKJNICall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jPlayerObj) {
    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jPlayerObj = jPlayerObj;
    initCreateAudioTrack();
    jclass jPlayerClass = jniEnv->GetObjectClass(jPlayerObj);
    jPlayerErrorMid = jniEnv->GetMethodID(jPlayerClass, "onError", "(ILjava/lang/String;)V");
    LOGI("----->");


}

LKJNICall::~LKJNICall() {
    jniEnv->DeleteLocalRef(jAudioTrackObj);
}


void LKJNICall::initCreateAudioTrack() {
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


    jclass jAudioTrackClass = jniEnv->FindClass("android/media/AudioTrack");
    jmethodID  jAudioTrackCMid = jniEnv->GetMethodID(jAudioTrackClass, "<init>", "(IIIIII)V");

    jmethodID getMinBufferSizeMid = jniEnv->GetStaticMethodID(jAudioTrackClass, "getMinBufferSize", "(III)I");
    bufferSizeInBytes = jniEnv->CallStaticIntMethod(jAudioTrackClass, getMinBufferSizeMid, sampleRateInHz, channelConfig, audioFormat);


    jAudioTrackObj = jniEnv->NewObject(jAudioTrackClass, jAudioTrackCMid, streamType, sampleRateInHz,
                                            channelConfig, audioFormat, bufferSizeInBytes, mode);

    // start play method
    jmethodID  playMid = jniEnv->GetMethodID(jAudioTrackClass, "play", "()V");
    jniEnv->CallVoidMethod(jAudioTrackObj, playMid);

    // write method
    jAudioTrackWriteMid = jniEnv->GetMethodID(jAudioTrackClass, "write", "([BII)I");
}

void LKJNICall::callAudioTrackWrite(jbyteArray audioData, int offsetInBytes, int sizeInBytes) {
    jniEnv->CallIntMethod(jAudioTrackObj, jAudioTrackWriteMid, audioData, offsetInBytes, sizeInBytes);
}

void LKJNICall::callPlayerJniError(int code, char *msg) {
    jstring  jMsg = jniEnv->NewStringUTF(msg);
    jniEnv->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
    LOGI("callPlayerJniError %d %s", code, msg);
    jniEnv->DeleteLocalRef(jMsg);

}


