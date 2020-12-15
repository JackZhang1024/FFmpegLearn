package com.luckyboy.ffmpeglearn;

import android.media.AudioTrack;
import android.provider.MediaStore;
import android.text.TextUtils;
import android.util.Log;

import com.luckyboy.ffmpeglearn.listener.MediaErrorListener;

public class FFmpegPlayer {


    static {
        // 因为是所有的都so都链接到一个.so文件(libnative-lib.so) 所以只能加载一个so文件
//        System.loadLibrary("avcodec");
//        System.loadLibrary("avdevice");
//        System.loadLibrary("avfilter");
//        System.loadLibrary("avformat");
//        System.loadLibrary("avutil");
//        System.loadLibrary("postproc");
//        System.loadLibrary("swresample");
//        System.loadLibrary("swscale");
        System.loadLibrary("media-player");
    }


    /**
     * url 可以是本地文件路径 也可以是http连接
     */
    private String url;
    private MediaErrorListener mErrorListener;

    public void setDataSource(String url) {
        this.url = url;
    }

    private static final String TAG = "FFmpegPlayer";

    private void onError(int code, String msg) {
        Log.e(TAG, "onError: Native callback "+code+" msg "+msg);
        if (mErrorListener != null) {
            mErrorListener.onError(code, msg);
        }
    }


    public void setErrorListener(MediaErrorListener mErrorListener) {
        this.mErrorListener = mErrorListener;
    }

    public void play() {
        if (TextUtils.isEmpty(url)) {
            throw new IllegalArgumentException("url is null");
        }
        playMedia(url);
    }

    // 按键option+回车就自动生成native方法
    public native void playMedia(String url);

}
