//
// Created by yanxi on 2019/8/25.
//
extern "C" {
#include <libavcodec/avcodec.h>
}

#include "XData.h"

void XData::Drop() {
    if (!data) {
        return;
    }
    //这里是把AVPacket本身内存释放掉  av_packet_unref只是释放对真正的音视频数据的引用，只释放真正的音视频数据
    //1.解引用数据data
    //2.销毁AVPacket本身对象
    //3.AVPacket的指针指向NULL
    av_packet_free(reinterpret_cast<AVPacket **>(&data));
    data = 0;
    size = 0;
}
