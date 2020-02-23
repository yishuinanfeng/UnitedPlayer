//
// Created by yanxi on 2019/8/25.
//
extern "C" {
#include <libavformat/avformat.h>
}

#include "FFDemux.h"
#include "XLog.h"

static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0. : (double) r.num / (double) r.den;
}

//打开文件或者流媒体 rmtp rtsp http
bool FFDemux::Open(const char *url) {
    Close();
    //  mux.lock();
    {
        const std::lock_guard<std::mutex> lock(mux);
        LOGI("open file %s", url);
        //创建了一个AVFormatContext,并读取文件的头部信息
        int re = avformat_open_input(&ic, url, 0, 0);
        if (re != 0) {
            char buf[1024] = {0};
            av_strerror(re, buf, sizeof(buf));
            LOGE("FFDemux open %s failed!", url);
            //     mux.unlock();
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
            //   mux.unlock();
            return false;
        }
        LOGI("avformat_find_stream_info %s success!", url);

        this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
        LOGI("avformat_find_stream_info  totalMs:%d", totalMs);
        //获取video和audio stream的index
        //  mux.unlock();
        //get里面有其他的锁，这里在之前unlock防止死锁
    }
    GetAudioParameter();
    GetVideoParameter();
    return true;
}

/**
 * 找出视频流videoStreamIndex和参数信息
 * @return
 */
XParameter FFDemux::GetVideoParameter() {
    //   mux.lock();
    const std::lock_guard<std::mutex> lock(mux);
    if (!ic) {
        LOGE("GetVideoParameter failed!ic is null");
        return XParameter();
    }
    //找到视频流信息
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (re < 0) {
        LOGE("av_find_best_stream failed!");
        //  mux.unlock();
        return XParameter();
    }
    videoStreamIndex = re;
    XParameter xParameter;
    xParameter.parameters = ic->streams[re]->codecpar;
    // mux.unlock();
    return xParameter;
}

/**
 * 找出音频流audioStreamIndex和参数信息
 * @return
 */
XParameter FFDemux::GetAudioParameter() {
    //  mux.lock();
    const std::lock_guard<std::mutex> lock(mux);
    if (!ic) {
        LOGE("GetVideoParameter failed!ic is null");
        return XParameter();
    }
    //找到音频流流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (re < 0) {
        LOGE("av_find_best_stream failed!");
        //   mux.unlock();
        return XParameter();
    }
    audioStreamIndex = re;
    XParameter xParameter;
    xParameter.parameters = ic->streams[re]->codecpar;
    xParameter.channels = ic->streams[re]->codecpar->channels;
    xParameter.sample_rate = ic->streams[re]->codecpar->sample_rate;
//    mux.unlock();
    return xParameter;
}

/**
 * 解复用一帧数据（注意防止内存泄漏），封装为XData对象
 */
XData FFDemux::Read() {
    //  mux.lock();
    const std::lock_guard<std::mutex> lock(mux);
    XData data;
    if (!ic) {
        //  mux.unlock();
        return XData();
    }
    AVPacket *pkt = av_packet_alloc();
    //从流中读取一帧，读入传入的AVPacket中
    int re = av_read_frame(ic, pkt);
    if (re != 0) {
        LOGDemux("解复用读取一帧失败");
        av_packet_free(&pkt);
        return XData();
    }
    LOGDemux("解复用读取一帧成功");
    LOGI("packet size:%d pts:%lld", pkt->size, pkt->pts);

    data.data = reinterpret_cast<unsigned char *>(pkt);
    data.size = pkt->size;
    if (pkt->stream_index == videoStreamIndex) {
        LOGDemux("视频帧");
        data.isAudio = false;
    } else if (pkt->stream_index == audioStreamIndex) {
        LOGDemux("音频帧");
        data.isAudio = true;
    } else {
        av_packet_free(&pkt);
        return XData();
    }
    //time_base单位为秒，这里转为毫秒
    //播放时间戳
    pkt->pts = static_cast<int64_t>(pkt->pts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base)));
    //解码时间戳（对于视频的）
    pkt->dts = static_cast<int64_t>(pkt->dts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base)));
    data.pts = static_cast<int>(pkt->pts);
    LOGD("demux pts %d", data.pts);

    return data;
}

FFDemux::FFDemux() {
    threadName = "FFDemux";

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

void FFDemux::Close() {
    const std::lock_guard<std::mutex> lock(mux);
    if (ic) {
        avformat_close_input(&ic);
    }
}

bool FFDemux::Seek(double position) {
    if (position < 0 || position > 1) {
        return false;
    }
    bool re;
    //   mux.lock();
    const std::lock_guard<std::mutex> lock(mux);
    if (!ic) {
      //  mux.unlock();
        return false;
    }
    avformat_flush(ic);
    long long seekPts = static_cast<long long int>(ic->streams[videoStreamIndex]->duration *
                                                   position);
    //AVSEEK_FLAG_BACKWARD： 往靠近视频开始的位置寻找关键帧
    //AVSEEK_FLAG_FRAME：基于帧查找
    re = av_seek_frame(ic, videoStreamIndex, seekPts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
    //   mux.unlock();
    return re;
}


