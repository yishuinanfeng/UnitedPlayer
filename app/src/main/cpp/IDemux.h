//
// Created by yanxi on 2019/8/25.
//

#ifndef MANCHESTERUNITEDPLAYER_IDEMUX_H
#define MANCHESTERUNITEDPLAYER_IDEMUX_H

#include "XData.h"

//解封装接口类
class IDemux {

public://？
    //打开文件或者流媒体 rmtp rtsp http
    virtual bool Open(const char* url) = 0;

    //读取一帧数据（注意防止内存泄漏）
    virtual XData Read() = 0;
    //总时长（毫秒）
    int totalMs = 0;
};


#endif //MANCHESTERUNITEDPLAYER_IDEMUX_H
