//
// Created by yanxi on 2019/8/25.
//

#ifndef MANCHESTERUNITEDPLAYER_FFDEMUX_H
#define MANCHESTERUNITEDPLAYER_FFDEMUX_H


#include "XData.h"
#include "IDemux.h"

struct AVFormatContext;

class FFDemux : public IDemux {

public:
    //打开文件或者流媒体 rmtp rtsp http
    virtual bool Open(const char *url);

    virtual void Close();

    //获取视频参数
    virtual XParameter GetVideoParameter();

    //获取音频参数
    virtual XParameter GetAudioParameter();

    //读取一帧数据（注意防止内存泄漏）
    virtual XData Read();

    FFDemux();

private:
    //整个音视频文件的信息
    AVFormatContext *ic = 0;
    std::mutex mux;
    int audioStreamIndex = 1;
    int videoStreamIndex = 0;
};


#endif //MANCHESTERUNITEDPLAYER_FFDEMUX_H
