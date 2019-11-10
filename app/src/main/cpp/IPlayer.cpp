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
    Close();
    mutex.lock();
    if (!iDemux || !iDemux->Open(path)) {
        LOGE("demux open %s fail", path);
        //return都要unlock
        mutex.unlock();
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
    mutex.unlock();
    return true;
}

bool IPlayer::Start() {

    //    iDemux->Start();
//    audioDecode->Start();
//    videoDecode->Start();
    mutex.lock();
    if (audioPlay) {
        audioPlay->StartPlay(outPara);
    }
    if (videoDecode) {
        videoDecode->Start();
    }
    if (audioDecode) {
        audioDecode->Start();
    }

    if (!iDemux || !iDemux->Start()) {
        LOGE("iDemux->Start fail");
        //return都要unlock
        mutex.unlock();
        return false;
    }

    XThread::Start();
    mutex.unlock();
    return true;
}

IPlayer::IPlayer() {

}

bool IPlayer::InitView(void *win) {
    if (videoView) {
        videoView->Close();
        videoView->SetRender(win);
    }
    return true;
}

void IPlayer::Main() {
    while (!isExit) {
        mutex.lock();
        //在player中将当前音频player的当前帧的pts赋值给视频解码器
        if (!audioPlay || !videoDecode) {
            mutex.unlock();
            continue;
        }
        //将当前音频帧pts赋值给视频解码器的synPts
        int apts = audioPlay->pst;
        videoDecode->synPts = apts;
        LOGD("apts = %d", apts);
        mutex.unlock();
        Sleep(2);
    }

}

void IPlayer::Close() {
    mutex.lock();
    //停止线程
    //同步线程
    XThread::Stop();
    if (iDemux) {
        iDemux->Stop();
    }
    if (videoDecode) {
        videoDecode->Stop();
    }
    if (audioDecode) {
        audioDecode->Stop();
    }
    //清理缓冲队列
    if (audioDecode) {
        audioDecode->Clear();
    }
    if (videoDecode) {
        videoDecode->Clear();
    }
    if (audioPlay) {
        audioPlay->Clear();
    }

    //清理资源
    if (audioDecode) {
        audioDecode->Close();
    }
    if (videoDecode) {
        videoDecode->Close();
    }
    if (audioPlay) {
        audioPlay->Close();
    }
    if (videoView) {
        videoView->Close();
    }
    if (iDemux) {
        iDemux->Close();
    }

    mutex.unlock();
}

double IPlayer::GetPlayPose() {
    double pos = 0.0;
    mutex.lock();
    int total = 0;
    if (iDemux){
        total = iDemux->totalMs;
    }
    if (total > 0){
        if (videoDecode){
            pos = (double)videoDecode->pts/(double)total;
        }
    }
    mutex.unlock();
    return pos;
}

bool IPlayer::Seek(double position) {
    bool re = false;
    mutex.lock();
    if (iDemux){
        re = iDemux->Seek(position);
    }
    mutex.unlock();
    return re;
}


