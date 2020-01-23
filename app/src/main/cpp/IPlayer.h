//
// Created by yanxi on 2019/10/26.
//

#ifndef MANCHESTERUNITEDPLAYER_IPLAYER_H
#define MANCHESTERUNITEDPLAYER_IPLAYER_H

#include <mutex>
#include "XThread.h"
#include "XParameter.h"

//不引用头文件，这样子可以使得外部调用者使用当前类的以下成员时候只需要引入当前类的头文件？
//而不需要引入下面类的头文件？
class IDemux;

class IDecode;

class IResample;

class IVideoView;

class IAudioPlay;
//todo 为什么这里给其他类使用会链接失败？？？
//int videoPts = 0;

/**
 * 整个播放器的入口类，屏蔽内部具体技术实现
 */
class IPlayer : public XThread {
public:
    //index为窗口索引
    static IPlayer *Get(unsigned int index = 0);

    virtual bool Open(const char *path);

    virtual void Close();

    virtual bool Start();

    virtual bool InitView(void *win, int filterType,int screenWidth, int screenHeight);

    //获取当前进度（0.0--1.0）
    virtual double GetPlayPose();

    virtual bool Seek(double position);

    virtual void SetPause(bool isP);

    virtual void setFilterType(int filterType);

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

protected:
    //用作音视频同步
    void Main();

    std::mutex mutex;

    IPlayer();
};


#endif //MANCHESTERUNITEDPLAYER_IPLAYER_H
