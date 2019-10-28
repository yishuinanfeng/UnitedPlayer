//
// Created by yanxi on 2019/10/26.
//

#ifndef MANCHESTERUNITEDPLAYER_IPLAYER_H
#define MANCHESTERUNITEDPLAYER_IPLAYER_H

#include "XThread.h"
#include "XParameter.h"

//不引用头文件，这样子可以使得外部调用者使用当前类的以下成员时候只需要引入当前类的头文件？
//而不需要引入下面类的头文件？
class IDemux;

class IDecode;

class IResample;

class IVideoView;

class IAudioPlay;

/**
 * 整个播放器的入口类，屏蔽内部具体技术实现
 */
class IPlayer : public XThread {
public:
    //index为窗口索引
    static IPlayer *Get(unsigned int index = 0);

    virtual bool Open(const char *path);

    virtual bool Start();

    virtual bool InitView(void *win);

//protected:
    IDemux *iDemux = 0;
    IDecode *videoDecode = 0;
    IDecode *audioDecode = 0;
    IResample *resample = 0;
    IVideoView *videoView = 0;
    IAudioPlay *audioPlay = 0;
    //音频输出参数
    XParameter outPara;

    bool isHardDecode = true;

    IPlayer();
};


#endif //MANCHESTERUNITEDPLAYER_IPLAYER_H
