//
// Created by 张凤洲 on 2020/12/13.
//

#ifndef FFMPEGLEARN_LKJNICALL_H
#define FFMPEGLEARN_LKJNICALL_H


#include <jni.h>

class LKJNICall {
public:
    jobject jAudioTrackObj;
    jmethodID jAudioTrackWriteMid;
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    jmethodID jPlayerErrorMid;
    jobject jPlayerObj;

public:
    LKJNICall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jPlayerObj);
    ~LKJNICall();

private:
    void initCreateAudioTrack();


public:
    void callAudioTrackWrite(jbyteArray audioData, int offsetInBytes, int sizeInBytes );

    void callPlayerJniError(int i, char *string);
};




#endif //FFMPEGLEARN_LKJNICALL_H
