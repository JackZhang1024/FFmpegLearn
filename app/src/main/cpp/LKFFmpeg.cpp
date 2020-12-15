//
// Created by 张凤洲 on 2020/12/15.
//

#include "LKFFmpeg.h"

LKFFmpeg::LKFFmpeg(LKJNICall *pJniCall, const char *url) {
    this->pJniCall = pJniCall;
    this->url = url;
}

LKFFmpeg::~LKFFmpeg() {
    release();
}

void LKFFmpeg::play() {
    LOGI("url:%s", url);
    av_register_all();
    // 初始化网络
    avformat_network_init();
    AVCodec *pCodec = NULL;
    int formatOpenInputRes = 0;
    int formatFindStreamInfoRes = 0;
    int audioStreamIndex = -1;
    AVCodecParameters *pCodecParameters = NULL;
    AVCodecContext *pCodecContext = NULL;
    int codecParameterToContextRes = -1;
    int avCodecOpenRes = -1;
    AVPacket *pPacket = NULL;
    AVFrame *pFrame = NULL;
    int index = 0;
    int64_t out_ch_layout;
    enum  AVSampleFormat out_sample_fmt;
    int out_sample_rate;
    int64_t  in_ch_layout;
    enum AVSampleFormat  in_sample_fmt;
    int  in_sample_rate;

    formatOpenInputRes = avformat_open_input(&pFormatContext, url, NULL, NULL);
    if (formatOpenInputRes != 0) {
        // 第一件事，需要回调给Java层
        // 第二件事，需要释放资源
        // av_err2str 根据返回值来判断错误的类型
        LOGI("format open input error:%s", av_err2str(formatOpenInputRes));
        callPlayerJniError(formatOpenInputRes, av_err2str(formatOpenInputRes));
        return;
        // 为什么要调回给Java Native层不能干嘛？
    }
    //avformat_find_stream_info(pFormatContext, NULL);
    formatFindStreamInfoRes = avformat_find_stream_info(pFormatContext, NULL);
    if (formatFindStreamInfoRes < 0) {
        // 一般不推荐这么处理
        LOGI("find stream info error:%s", av_err2str(formatFindStreamInfoRes));
        callPlayerJniError(formatFindStreamInfoRes, av_err2str(formatFindStreamInfoRes));
        return;
    }
    audioStreamIndex = av_find_best_stream(pFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                           NULL, 0);
    LOGE("audioStreamIndex %d", audioStreamIndex);
    if (audioStreamIndex < 0) {
        // 一般不推荐这么处理
        LOGI("format find audio stream error:%s", "format audio stream error");
        callPlayerJniError(FIND_STREAM_ERROR_CODE, av_err2str(audioStreamIndex));
        return;
    }
    // 查找解码器
    pCodecParameters = pFormatContext->streams[audioStreamIndex]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        // 一般不推荐这么处理
        LOGI("codec find audio decoder error:%s", av_err2str(formatOpenInputRes));
        callPlayerJniError(CODEC_FIND_DECODER_ERROR_CODE, "codec find audio decoder error");
        return;
    }
    // 打开解码器
    pCodecContext = avcodec_alloc_context3(pCodec);
    if (pCodecContext == NULL) {
        // 一般不推荐这么处理
        LOGI("codec_alloc_context error");
        callPlayerJniError(CODEC_ALLOC_CONTEXT_ERROR_CODE, "codec_alloc_context errorr");
        return;
    }
    codecParameterToContextRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codecParameterToContextRes < 0) {
        // 一般不推荐这么处理
        LOGI("codec parameter to_context error:%s", av_err2str(codecParameterToContextRes));
        callPlayerJniError(codecParameterToContextRes, av_err2str(codecParameterToContextRes));
        return;
    }
    avCodecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);
    if (avCodecOpenRes != 0) {
        // 一般不推荐这么处理
        LOGI("codec audio open error:%s", av_err2str(avCodecOpenRes));
        callPlayerJniError(avCodecOpenRes, av_err2str(avCodecOpenRes));
        return;
    }
    LOGE("%d,%d", pCodecParameters->sample_rate, pCodecParameters->channels);
    // 解码一帧 播放一帧
    //jAudioTrackClass = pJniCall->jniEnv->FindClass("android/media/AudioTrack");
    //  public int write(@NonNull byte[] audioData, int offsetInBytes, int sizeInBytes)
    //jWriteCMid = pJniCall->jniEnv->GetMethodID(jAudioTrackClass, "write", "([BII)I");


    // 重采样 开始
    // 设置重采样参数
    // struct SwrContext *swr_alloc_set_opts(struct SwrContext *s,
    // int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
    // int64_t  in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
    // int log_offset, void *log_ctx);
    // 输出通道类型
    out_ch_layout = AV_CH_LAYOUT_STEREO;
    // 输出采样类型
    out_sample_fmt = AVSampleFormat ::AV_SAMPLE_FMT_S16;
    out_sample_rate = AUDIO_SAMPLE_RATE;
    // 输入的通道
    in_ch_layout = pCodecContext->channel_layout;
    in_sample_fmt = pCodecContext->sample_fmt;
    in_sample_rate = pCodecContext->sample_rate;
    // 设置重采样参数
    swrContext = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_fmt, out_sample_rate,
                                    in_ch_layout, in_sample_fmt, in_sample_rate,
                                    0,
                                    NULL
    );
    if (swrContext == NULL){
        // 待处理
        callPlayerJniError(SWR_ALLOC_SET_OPTS_ERROR_CODE, "swr_alloc_set_opts error");
        return;
    }
    int swrInitRes =  swr_init(swrContext);
    if (swrInitRes <0 ){
        callPlayerJniError(SWR_INIT_ERROR_CODE, "swr_init error");
        return;
    }
    /// size是指定播放的大小 是最终输出的大小
    int outChannels = av_get_channel_layout_nb_channels(out_ch_layout);
    int dataSize = av_samples_get_buffer_size(NULL, outChannels,pCodecParameters->frame_size,
                                              out_sample_fmt, 0);
    LOGI("init DataSize %d", dataSize);

    // 指定输出到哪里
    resampleOutBuffer = (uint8_t*)malloc(dataSize);
    // 重采样 结束

    jbyteArray jPcmByteArray = pJniCall->jniEnv->NewByteArray(dataSize);
    jbyte  *jPcmData= pJniCall->jniEnv->GetByteArrayElements(jPcmByteArray, NULL);

    pPacket = av_packet_alloc();
    pFrame = av_frame_alloc();

    while (av_read_frame(pFormatContext, pPacket) >= 0) {
        //Packet 包 压缩的数据 解码生成 pcm数据
        // 判断是否是音频数据
        if (pPacket->stream_index == audioStreamIndex) {
            // Supply raw packet data as input to a decoder. 发送原始数据到decoder
            int codecSendPacketRes = avcodec_send_packet(pCodecContext, pPacket);
            if (codecSendPacketRes == 0) {
                // eturn decoded output data from a decoder. AvFrame是解码好的存放的数据
                int codecReceiveFrameRes = avcodec_receive_frame(pCodecContext, pFrame);
                if (codecReceiveFrameRes == 0) {
                    // pcm数据

                    index++;
                    //LOGI("解码第%d帧", index);

                    // 调用重采样的方法
                    swr_convert(swrContext, &resampleOutBuffer, pFrame->nb_samples, (const uint8_t **)pFrame->data, pFrame->nb_samples);

                    // write 写到缓冲区 pFrame.data->javabyte
                    // size 是多大 装pcm数据 pcm 在Frame中
                    // 44100点 2通道 2 字节 44100*2*2
                    // 1帧 不是1 秒 pFrame pFrame->nb_samples 点 是有 nb_samples个采样点

                    // int av_samples_get_buffer_size(int *linesize, int nb_channels, int nb_samples,
                    // enum AVSampleFormat sample_fmt, int align);

//                    int dataSize = av_samples_get_buffer_size(NULL, pFrame->channels, pFrame->nb_samples, pCodecContext->sample_fmt, 0);
//                    jbyteArray jPcmByteArray = env->NewByteArray(dataSize);
                    // 把C的数据同步到Java这块来
                    // native 创建c数组

                    // 将pcm 数据拷贝到jPcmData中
                    // 有滋滋声 需要重新采样才可以解决
                    memcpy(jPcmData, resampleOutBuffer, dataSize);

                    // 0代表将c中的数据同步到jbyteArray 然后释放native数组
                    // 不释放 但是需要同步内存
                    pJniCall->jniEnv->ReleaseByteArrayElements(jPcmByteArray, jPcmData, JNI_COMMIT);

                    //env->CallIntMethod(jAudioTrackObj, jWriteCMid, jPcmByteArray, 0, dataSize);
                    pJniCall->callAudioTrackWrite(jPcmByteArray, 0, dataSize);
                }
            }
        }
        // 解引用
        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }


    // 1. 把数据置为空 2. 将指针置为NULL pPacket指针 指向了一个data数据
    // 1. 解引用数据data 2. 销毁pPacket 结构体内存 3. pPacket = NULL
    av_packet_free(&pPacket);
    // 1. 把数据置为空 2. 将指针置为NULL pFrame指针 指向了一个data数据
    av_frame_free(&pFrame);
    // 释放audioTrackObj
    pJniCall->jniEnv->ReleaseByteArrayElements(jPcmByteArray, jPcmData, 0);
    pJniCall->jniEnv->DeleteLocalRef(jPcmByteArray);
}

void LKFFmpeg::callPlayerJniError(int code, char *msg) {
    // 释放资源
    release();
    // 回调给Java层使用
    pJniCall->callPlayerJniError(code, msg);
}

void LKFFmpeg::release() {
    LOGI("LKFFmpeg release....");
    if (pCodecContext != NULL) {
        // 释放里面
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
        // 释放自己
        pCodecContext = NULL;
    }
    if (pFormatContext != NULL) {
        avformat_close_input(&pFormatContext);
        // 释放里面
        avformat_free_context(pFormatContext);
        // 释放自己
        pFormatContext = NULL;
    }
    if (swrContext!=NULL){
        swr_free(&swrContext);
        free(&swrContext);
        swrContext = NULL;
    }
    if (resampleOutBuffer!=NULL){
        free(resampleOutBuffer);
        resampleOutBuffer = NULL;
    }
    avformat_network_deinit();
}


