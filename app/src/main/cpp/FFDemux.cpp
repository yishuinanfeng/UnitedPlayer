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
    //创建了一个AVFormatContext,并读取文件的头部信息
    int re = avformat_open_input(&ic, url, 0, 0);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("FFDemux open %s failed!", url);
        return false;
    }
    LOGI("FFDemux open %s success!", url);
    //得到流的上下文，读取流信息
    re = avformat_find_stream_info(ic, 0);
    //>=0 is ok?
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("avformat_find_stream_info %s failed!", url);
        return false;
    }
    LOGI("avformat_find_stream_info %s success!", url);

    this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
    LOGI("avformat_find_stream_info  totalMs:%d", totalMs);
    //获取video和audio stream的index
    GetAudioParameter();
    GetVideoParameter();
    return true;
}
/**
 * 找出视频流videoStreamIndex和参数信息
 * @return
 */
XParameter FFDemux::GetVideoParameter() {
    if (!ic) {
        LOGE("GetVideoParameter failed!ic is null");
        return XParameter();
    }
    //找到视频流信息
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (re < 0) {
        LOGE("av_find_best_stream failed!");
        return XParameter();
    }
    videoStreamIndex = re;
    XParameter xParameter;
    xParameter.parameters = ic->streams[re]->codecpar;
    return xParameter;
}
/**
 * 找出音频流audioStreamIndex和参数信息
 * @return
 */
XParameter FFDemux::GetAudioParameter() {
    if (!ic) {
        LOGE("GetVideoParameter failed!ic is null");
        return XParameter();
    }
    //找到音频流流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (re < 0) {
        LOGE("av_find_best_stream failed!");
        return XParameter();
    }
    audioStreamIndex = re;
    XParameter xParameter;
    xParameter.parameters = ic->streams[re]->codecpar;
    return xParameter;
}

/**
 * 解复用一帧数据（注意防止内存泄漏），封装为XData对象
 */
XData FFDemux::Read() {
    XData data;
    if (!ic) {
        return XData();
    }
    AVPacket *pkt = av_packet_alloc();
    //从流中读取一帧，读入传入的AVPacket中
    int re = av_read_frame(ic, pkt);
    if (re != 0) {
        av_packet_free(&pkt);
        return XData();
    }
    LOGI("packet size:%d pts:%lld", pkt->size, pkt->pts);

    data.data = reinterpret_cast<unsigned char *>(pkt);
    data.size = pkt->size;
    if (pkt->stream_index == videoStreamIndex) {
        data.isAudio = false;
    } else if (pkt->stream_index == audioStreamIndex) {
        data.isAudio = true;
    } else {
        av_packet_free(&pkt);
        return XData();
    }
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


