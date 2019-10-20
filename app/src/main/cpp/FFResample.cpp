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
    swrContext = swr_alloc();
    AVCodecParameters *p = in.parameters;
    //参数前一部分是输出，后一部分是输入
    swrContext = swr_alloc_set_opts(swrContext, av_get_default_channel_layout(2),
                                    AV_SAMPLE_FMT_S16,
                                    p->sample_rate,
                                    av_get_default_channel_layout(p->channels),
                                    static_cast<AVSampleFormat>(p->format),
                                    p->sample_rate, 0, 0);
    int re = swr_init(swrContext);
    if (re != 0) {
        LOGE("swr init fail");
        return false;
    }
    LOGD("swr init success");
    outChannels = p->channels;
    outFormat = AV_SAMPLE_FMT_S16;
    return true;
}

XData FFResample::Resample(XData xData) {
    LOGD("Resample XData size:%d", xData.size);
    if (xData.size <= 0 || !xData.data) {
        return XData();
    }
    if (!swrContext) {
        return XData();
    }
    //将data转化为一个AVFrame（一帧）
    AVFrame *frame = reinterpret_cast<AVFrame *>(xData.data);
    XData out;
    //一帧几个字节
    int outSize = outChannels * frame->nb_samples * av_get_bytes_per_sample(
            static_cast<AVSampleFormat>(outFormat));
    if (outSize <= 0) {
        return XData();
    }
    out.Alloc(outSize);
    uint8_t *outArr[2] = {0};
    //重采样后的数据存放在out.data
    outArr[0] = out.data;
    //todo 为什么转型不能用retraincast?
    //对一帧的音频进行重采样。 frame->nb_samples表示单通道的样本数量
    int len = swr_convert(swrContext, outArr, frame->nb_samples,
                          (const uint8_t **) (frame->data), frame->nb_samples);
    if (len <= 0) {
        out.Drop();
        return XData();
    }
    LOGD("Resample success：%d", outSize);
    return out;
}
