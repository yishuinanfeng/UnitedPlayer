//
// Created by yanxi on 2019/9/1.
//
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/jni.h>
}


#include "FFDecode.h"
#include "XLog.h"

void FFDecode::InitHard(void *vm) {
    av_jni_set_java_vm(vm, 0);
    LOGD("InitHard");
}


/**
 * 打开解码器
 * @param xParameter
 * @return
 */
bool FFDecode::Open(XParameter xParameter, bool isHard) {
    Close();
    if (!xParameter.parameters) {
        return false;
    }
    //1.查找解码器
    AVCodecParameters *p = xParameter.parameters;
    AVCodec *cd = avcodec_find_decoder(p->codec_id);
    if (isHard) {
        cd = avcodec_find_decoder_by_name("h264_mediacodec");
    }
    if (!cd) {
        LOGE("avcodec_find_decoder %d failed", p->codec_id, isHard);
        return false;
    }
    LOGI("avcodec_find_decoder success! %d", isHard);
    mutex1.lock();
    //2 创建解码器上下文，并复制参数
    codecContext = avcodec_alloc_context3(cd);
    //因为新的版本已经将AVStream结构体中的AVCodecContext字段定义为废弃属性，
    // 所以现在是通过AVCodecParameters获取AVCodecContext
    avcodec_parameters_to_context(codecContext, p);
    //开8个线程
    codecContext->thread_count = 8;
    //打开解码器
    int re = avcodec_open2(codecContext, 0, 0);
    LOGI("avcodec_open2 success!");
    if (re != 0) {
        mutex1.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        LOGE("%s", buf);
        return false;
    }

    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        this->isAudio = false;
        LOGE("Open isAudio %d", false);
    } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        this->isAudio = true;
        LOGE("Open isAudio %d", true);
    }
    mutex1.unlock();
    return true;
}

bool FFDecode::SendPacket(XData pkt) {
    mutex1.lock();
    if (!codecContext) {
        mutex1.unlock();
        return false;
    }
    if (pkt.size <= 0 || !pkt.data) {
        mutex1.unlock();
        return false;
    }
    //发送到解码线程进行解码
    int re = avcodec_send_packet(codecContext, reinterpret_cast<const AVPacket *>(pkt.data));
    mutex1.unlock();
    return re == 0;
}

/**
 * 具体的解码操作，从线程中获取解码结果,再次调用会复用上次内存空间，线程不安全
 */
XData FFDecode::RecvFrame() {
    mutex1.lock();
    if (!codecContext) {
        mutex1.unlock();
        return XData();
    }
    if (!avFrame) {
        avFrame = av_frame_alloc();
    }
    //得到解码之后的一帧，将avcodec_send_packet的一帧解码到avFrame
    int re = avcodec_receive_frame(codecContext, avFrame);
    if (re != 0) {
        mutex1.unlock();
        return XData();
    }
    XData xData;
    //todo 直接强转？
    xData.data = reinterpret_cast<unsigned char *>(avFrame);
    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        xData.size = (avFrame->linesize[0] + avFrame->linesize[1] + avFrame->linesize[2]) *
                     avFrame->height;
        xData.width = avFrame->width;
        xData.height = avFrame->height;
    } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        //样本字节数*样本数*声道数
        xData.size = av_get_bytes_per_sample(static_cast<AVSampleFormat>(avFrame->format)) *
                     avFrame->nb_samples * 2;
    }
    xData.format = avFrame->format;
    if (!isAudio) {
        LOGD("data format is %d", avFrame->format);
    }
    //将avFrame的具体音视频数据拷贝到xData.datas中（两个数组大小都是8）
    memcpy(xData.datas, avFrame->data, sizeof(xData.datas));
    xData.pts = avFrame->pts;
    mutex1.unlock();
    return xData;
}

void FFDecode::Close() {
    Clear();
    mutex1.lock();
    pts = 0;
    if (avFrame) {
        //函数中会自动设置为null
        av_frame_free(&avFrame);
    }
    if (codecContext) {
        avcodec_close(codecContext);
        avcodec_free_context(&codecContext);
    }
    mutex1.unlock();
}



