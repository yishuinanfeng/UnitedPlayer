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
        //使用硬解码
        cd = avcodec_find_decoder_by_name("h264_mediacodec");
    }
    if (!cd) {
        LOGE("avcodec_find_decoder %d failed", p->codec_id, isHard);
        return false;
    }
    LOGI("avcodec_find_decoder success! %d", isHard);
    // mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);

    //2 创建解码器上下文，并复制参数
    codecContext = avcodec_alloc_context3(cd);
    //因为新的版本已经将AVStream结构体中的AVCodecContext字段定义为废弃属性，
    // 所以现在是通过AVCodecParameters获取AVCodecContext
    avcodec_parameters_to_context(codecContext, p);
    //开8个线程解码
    codecContext->thread_count = 8;
    //打开解码器
    int re = avcodec_open2(codecContext, 0, 0);
    LOGDecode("avcodec_open2 success!");
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        LOGDecode("%s", buf);
        return false;
    }

    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        this->isAudio = false;
        LOGDecode("Open isAudio %d", false);
    } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        this->isAudio = true;
        LOGDecode("Open isAudio %d", true);
    }

    return true;
}

bool FFDecode::SendPacket(XData pkt) {

    const std::lock_guard<std::mutex> lock(mutex1);

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

/**
 * 具体的解码操作，从线程中获取解码结果,再次调用会复用上次内存空间，线程不安全
 */
XData FFDecode::RecvFrame() {
    //   mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);

    if (!codecContext) {
        //   mutex1.unlock();
        return XData();
    }
    if (!avFrame) {
        avFrame = av_frame_alloc();
    }
    //得到解码之后的一帧，将avcodec_send_packet的一帧解码到avFrame
    int re = avcodec_receive_frame(codecContext, avFrame);
    if (re != 0) {
        LOGDecode("解码一帧失败");
        return XData();
    }
    LOGDecode("解码一帧成功");
    XData xData;
    //todo 直接强转？
    xData.data = reinterpret_cast<unsigned char *>(avFrame);
    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        LOGDecode("视频帧处理");
        xData.size = (avFrame->linesize[0] + avFrame->linesize[1] + avFrame->linesize[2]) *
                     avFrame->height;
        xData.width = avFrame->width;
        xData.height = avFrame->height;
    } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        LOGDecode("音频帧处理");
        //样本字节数*样本数*声道数
        xData.size = av_get_bytes_per_sample(static_cast<AVSampleFormat>(avFrame->format)) *
                     avFrame->nb_samples * 2;
    }
    xData.format = avFrame->format;
    if (!isAudio) {
        LOGDecode("video format:%d", xData.format);
        LOGDecode("data format is %d", avFrame->format);
    }
    //将avFrame的具体音视频数据拷贝到xData.datas中（两个数组大小都是8）
    memcpy(xData.datas, avFrame->data, sizeof(xData.datas));
    xData.pts = static_cast<int>(avFrame->pts);
    return xData;
}

void FFDecode::Close() {
    Clear();
    const std::lock_guard<std::mutex> lock(mutex1);

    pts = 0;
    if (avFrame) {
        //函数中会自动设置为null
        av_frame_free(&avFrame);
    }
    if (codecContext) {
        avcodec_close(codecContext);
        avcodec_free_context(&codecContext);
    }
}

void FFDecode::Clear() {
    IDecode::Clear();
    //   mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);

    if (codecContext) {
        //清空ffmpeg内部的解码缓冲队列
        avcodec_flush_buffers(codecContext);
    }
    //  mutex1.unlock();

}

FFDecode::FFDecode() {
    threadName = "FFDecode";
}



