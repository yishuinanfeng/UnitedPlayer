//
// Created by yanxi on 2019/10/26.
//
//但是调用"IPlayer.h"中的成员的方法还是要引入头文件
#include "IPlayer.h"
#include "IDemux.h"
#include "XLog.h"
#include "IDecode.h"
#include "IResample.h"
#include "IAudioPlay.h"
#include "IVideoView.h"

//最推荐的懒汉式单例(magic static )——局部静态变量,是线程安全的（为什么？）
IPlayer *IPlayer::Get(unsigned int index) {
    static IPlayer p[256];
    return &p[index];
}

bool IPlayer::Open(const char *path) {
    if (!iDemux || !iDemux->Open(path)) {
        LOGE("demux open %s fail", path);
        return false;
    }
    //下面这几个之所以没有return false，是因为可能是不需要解码或重采样的原始数据
    if (!videoDecode || !videoDecode->Open(iDemux->GetVideoParameter())) {
        LOGE("videoDecode open %s fail", path);
    }
    if (!audioDecode || !audioDecode->Open(iDemux->GetAudioParameter())) {
        LOGE("audioDecode open %s fail", path);
    }
//    if (outPara){判断是否已经配置过（有必要？）
//
//    }
    outPara = iDemux->GetAudioParameter();
    if (!resample || !resample->Open(iDemux->GetAudioParameter(), outPara)) {
        LOGE("resample open %s fail", path);
    }
    return true;
}

bool IPlayer::Start() {

    //    iDemux->Start();
//    audioDecode->Start();
//    videoDecode->Start();
    if (!iDemux || !iDemux->Start()) {
        LOGE("iDemux->Start fail");
        return false;
    }
    if (audioDecode) {
        audioDecode->Start();
    }
    if (audioPlay) {
        audioPlay->StartPlay(outPara);
    }
    if (videoDecode) {
        videoDecode->Start();
    }
    return true;
}

IPlayer::IPlayer() {

}

bool IPlayer::InitView(void *win) {
    if (videoView) {
        videoView->SetRender(win);
    }
    return true;
}


