//
// Created by yanxi on 2019/10/19.
//
extern "C" {
#include <libswresample/swresample.h>
}

#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavcodec/avcodec.h>
#include "FFResample.h"
#include "XLog.h"

bool FFResample::Open(XParameter in, XParameter out) {
    Close();
  //  mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);
    swrContext = swr_alloc();
    AVCodecParameters *p = in.parameters;
    //参数前一部分是输出，后一部分是输入
    //这里声道数和采样率不变，只修改采样格式
    swrContext = swr_alloc_set_opts(swrContext, av_get_default_channel_layout(out.channels),
                                    AV_SAMPLE_FMT_S16,
                                    out.sample_rate,
                                    av_get_default_channel_layout(p->channels),
                                    static_cast<AVSampleFormat>(p->format),
                                    p->sample_rate, 0, 0);
    int re = swr_init(swrContext);
    if (re != 0) {
        LOGE("swr init fail");
   //     mutex1.unlock();
        return false;
    }
    LOGD("swr init success");
    outChannels = p->channels;
    outFormat = AV_SAMPLE_FMT_S16;
  //  mutex1.unlock();
    return true;
}

XData FFResample::Resample(XData xData) {
    //  LOGD("Resample XData size:%d", xData.size);
    if (xData.size <= 0 || !xData.data) {
        return XData();
    }
  //  mutex1.lock();
    LOGLOCK("Resample lock() start");
    const std::lock_guard<std::mutex> lock(mutex1);
    LOGLOCK("Resample lock() end");

    if (!swrContext) {
     //   mutex1.unlock();
        LOGLOCK("Resample unlock()1");
        return XData();
    }
    //将data转化为一个AVFrame（一帧）
    AVFrame *frame = reinterpret_cast<AVFrame *>(xData.data);
    XData out;
    //一帧几个字节
    int outSize = outChannels * frame->nb_samples * av_get_bytes_per_sample(
            static_cast<AVSampleFormat>(outFormat));
    if (outSize <= 0) {
     //   mutex1.unlock();
        LOGLOCK("Resample unlock()2");
        return XData();
    }
    out.Alloc(outSize);
    //存放两个uint8_t指针
    uint8_t *outArr[2] = {0};
    //第一个uint8_t指针等于out.data，所以重采样后的数据存放在out.data
    outArr[0] = out.data;
    //todo 为什么转型不能用retraincast?
    //对一帧的音频进行重采样。 frame->nb_samples表示单通道的样本数量
    int len = swr_convert(swrContext, outArr, frame->nb_samples,
                          (const uint8_t **) (frame->data), frame->nb_samples);
    if (len <= 0) {
        out.Drop();
     //   mutex1.unlock();
        return XData();
    }
    out.pts = xData.pts;
    //   LOGD("Resample success：%d", outSize);
//    mutex1.unlock();
    LOGLOCK("Resample unlock()3");
    return out;
}

void FFResample::Close() {
 //   mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);

    if (swrContext) {
        swr_free(&swrContext);
    }
 //   mutex1.unlock();
}
