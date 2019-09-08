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
    if (!cd) {
        LOGE("avcodec_find_decoder %d failed", p->codec_id);
        return false;
    }
    LOGI("avcodec_find_decoder success!");
    //2 创建解码器上下文，并复制参数
    codecContext = avcodec_alloc_context3(cd);
    //todo 这一步的意义与必要性？
    avcodec_parameters_to_context(codecContext, p);
    //开8个线程
    codecContext->thread_count = 8;
    //打开解码器
    int re = avcodec_open2(codecContext, 0, 0);
    LOGI("avcodec_open2 success!");
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        LOGE("%s", buf);
        return false;
    }

    return true;
}

bool FFDecode::SendPacket(XData pkt) {
    if (!codecContext) {
        return false;
    }
    if (pkt.size <= 0 || !pkt.data) {
        return false;
    }
    //发送到解码线程进行解码
    int re = avcodec_send_packet(codecContext, reinterpret_cast<const AVPacket *>(pkt.data));
    return re == 0;
}
//从线程中获取解码结果
XData FFDecode::RecvFrame() {
    if (!codecContext) {
        return XData();
    }
    if (!avFrame) {
        avFrame = av_frame_alloc();
    }
    //avFrame得到解码之后的一帧
    int re = avcodec_receive_frame(codecContext, avFrame);
    if (re != 0) {
        return XData();
    }
    XData xData;
    xData.data = reinterpret_cast<unsigned char *>(avFrame);
    xData.size = (avFrame->linesize[0] + avFrame->linesize[1] + avFrame->linesize[2]) * avFrame->height;
    return xData;
}

