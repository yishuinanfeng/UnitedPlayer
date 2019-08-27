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
    av_packet_free(reinterpret_cast<AVPacket **>(&data));
    data = 0;
    size = 0;
}
