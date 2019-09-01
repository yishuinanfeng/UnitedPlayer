//
// Created by yanxi on 2019/8/25.
//
extern "C" {
#include <libavformat/avformat.h>
}

#include "FFDemux.h"
#include "XLog.h"

//打开文件或者流媒体 rmtp rtsp http
bool FFDemux::Open(const char *url) {
    LOGI("open file %s", url);
    int re = avformat_open_input(&ic, url, 0, 0);
    if (re != 0){
        char buf[1024] = {0};
        av_strerror(re,buf, sizeof(buf));
        LOGE("FFDemux open %s failed!",url);
        return false;
    }
    LOGI("FFDemux open %s success!",url);

    re = avformat_find_stream_info(ic,0);
    if (re != 0){
        char buf[1024] = {0};
        av_strerror(re,buf, sizeof(buf));
        LOGE("avformat_find_stream_info %s failed!",url);
        return false;
    }
    LOGI("avformat_find_stream_info %s success!",url);

    this->totalMs = ic->duration/(AV_TIME_BASE/1000);
    LOGI("avformat_find_stream_info  totalMs:%d",totalMs);


    return true;
}

//读取一帧数据（注意防止内存泄漏）
XData FFDemux::Read() {
    XData data;
    if (!ic){
        return XData();
    }
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ic,pkt);
    if (re != 0){
        av_packet_free(&pkt);
        return XData();
    }
    LOGI("packet size:%d pts:%lld",pkt->size,pkt->pts);

    data.data = reinterpret_cast<unsigned char *>(pkt);
    data.size = pkt->size;
    return data;
}

FFDemux::FFDemux() {
    static bool isFirst = true;
    if (isFirst) {
        isFirst = false;
        //注册解封装器
        av_register_all();
        //注册所有的解码器
        avcodec_register_all();
        //初始化网络
        avformat_network_init();
        LOGI("register ffmpeg");
    }
}
