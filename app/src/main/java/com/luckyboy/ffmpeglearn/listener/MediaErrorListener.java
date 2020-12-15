package com.luckyboy.ffmpeglearn.listener;

// 错误的回调处理
public interface MediaErrorListener {

    void onError(int code, String msg);

}
