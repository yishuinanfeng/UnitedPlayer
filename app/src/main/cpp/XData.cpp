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
    if (type == AVPACKET_TYPE) {
        //这里是把AVPacket本身内存释放掉  av_packet_unref只是释放对真正的音视频数据的引用，只释放真正的音视频数据
        //1.解引用数据data
        //2.销毁AVPacket本身对象
        //3.AVPacket的指针指向NULL
        av_packet_free(reinterpret_cast<AVPacket **>(&data));
    } else {
        //其他类型通过delete释放内存
        delete data;
    }

    data = nullptr;
    size = 0;
}

bool XData::Alloc(int size, const char *d) {
    Drop();
    type = UCHAR_TYPE;
    if (size < 0) {
        return false;
    }
    this->data = new unsigned char[size];
    if (d) {
        memcpy(this->data, d, size);
    }
    this->size = size;
    return true;
}
