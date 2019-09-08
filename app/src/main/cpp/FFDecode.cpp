//
// Created by yanxi on 2019/9/1.
//
extern "C" {
    #include <libavcodec/avcodec.h>
}

#include "FFDecode.h"
#include "XLog.h"

bool FFDecode::Open(XParameter xParameter) {
    if (!xParameter.parameters) {
        return false;
    }
    //1.查找解码器
    AVCodecParameters *p = xParameter.parameters;
    AVCodec *cd = avcodec_find_decoder(p->codec_id);
    if (!cd){
        LOGE("avcodec_find_decoder %d failed",p->codec_id);
        return false;
    }
    LOGI("avcodec_find_decoder success!");
    //2 创建解码器上下文，并复制参数
    codecContext = avcodec_alloc_context3(cd);
    //todo 这一步的意义与必要性？
    avcodec_parameters_to_context(codecContext,p);
    //打开解码器
    int re = avcodec_open2(codecContext,0,0);
    LOGI("avcodec_open2 success!");
    if (re != 0){
        char buf[1024] = {0};
        av_strerror(re,buf, sizeof(buf) - 1);
        LOGE("%s",buf);
        return false;
    }

    return true;
}
