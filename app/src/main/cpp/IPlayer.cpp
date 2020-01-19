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
    // mutex.lock();
    const std::lock_guard<std::mutex> lock(mutex);

    if (!iDemux || !iDemux->Open(path)) {
        LOGE("demux open %s fail", path);
        //return都要unlock
        //    mutex.unlock();
        return false;
    }
    //下面这几个之所以没有return false，是因为可能是不需要解码或重采样的原始数据
    if (!videoDecode || !videoDecode->Open(iDemux->GetVideoParameter(), true)) {
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
    //   mutex.unlock();
    return true;
}

bool IPlayer::Start() {

    //    iDemux->Start();
//    audioDecode->Start();
//    videoDecode->Start();
    //  mutex.lock();
    const std::lock_guard<std::mutex> lock(mutex);

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
        //   mutex.unlock();
        return false;
    }

    XThread::Start();
    //  mutex.unlock();
    return true;
}

IPlayer::IPlayer() {

}

bool IPlayer::InitView(void *win, int filterType, int screenWidth, int screenHeight) {
    if (videoView) {
        videoView->Close();
        videoView->SetRender(win);
        videoView->setFilterType(filterType);
        videoView->setScreenDimension(screenWidth, screenHeight);
    }
    return true;
}

void IPlayer::Main() {
    while (!isExit) {
        //   mutex.lock();
        const std::lock_guard<std::mutex> lock(mutex);

        //在player中将当前音频player的当前帧的pts赋值给视频解码器
        if (!audioPlay || !videoDecode) {
            //     mutex.unlock();
            continue;
        }
        //将当前音频帧pts赋值给视频解码器的synPts
        int apts = audioPlay->pst;
        videoDecode->synPts = apts;
        LOGD("apts = %d", apts);
        //     mutex.unlock();
        Sleep(2);
    }

}

void IPlayer::Close() {
    //  mutex.lock();
    const std::lock_guard<std::mutex> lock(mutex);

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

    if (audioPlay) {
        audioPlay->Stop();
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

    //   mutex.unlock();
}

double IPlayer::GetPlayPose() {
    double pos = 0.0;
    LOGD("seek debug:GetPlayPose start");
    //   mutex.lock();
    const std::lock_guard<std::mutex> lock(mutex);

    int total = 0;
    if (iDemux) {
        total = iDemux->totalMs;
    }
    if (total > 0) {
        if (videoDecode) {
            pos = (double) videoDecode->pts / (double) total;
        }
    }
    //  mutex.unlock();
    LOGD("seek debug:GetPlayPose end");

    return pos;
}


bool IPlayer::Seek(double position) {
    if (!iDemux) {
        return false;
    }
    bool re;
    //seek之前暂停所有的线程
    SetPause(true);
    LOGS("seek debug:Seek start");

    // mutex.lock();
    {
        const std::lock_guard<std::mutex> lock(mutex);

        //清理各个模块缓冲队列
        LOGS("mutex.lock()");
        if (audioDecode) {
            audioDecode->Clear();
        }
        LOGS("audioDecode->Clear()");
        if (videoDecode) {
            videoDecode->Clear();
        }
        LOGS("videoDecode->Clear()");
        if (audioPlay) {
            audioPlay->Clear();
        }
        LOGS("audioPlay->Clear()");
        re = iDemux->Seek(position);
        LOGS("iDemux->Seek end");
        //假如没有视频，则函数返回
        if (!videoDecode) {
            //    mutex.unlock();
            //seek之后重新开启所有的线程
            SetPause(false);
            return re;
        }

        //在position时间点之前的帧都丢弃，从实际需要显示的帧开始
        int seekPts = position * iDemux->totalMs;
        while (!isExit) {
            XData pkt = iDemux->Read();
            if (pkt.size <= 0) {
                break;
            }

            if (pkt.isAudio) {
                //pts小于seekPts则丢弃
                if (pkt.pts < seekPts) {
                    pkt.Drop();
                    continue;
                }
                //pts不小于seekPts则交给audioDecode处理
                iDemux->notify(pkt);
                continue;
            }

            videoDecode->SendPacket(pkt);
            pkt.Drop();
            XData data = videoDecode->RecvFrame();
            if (data.size <= 0) {
                continue;
            }

            if (data.pts >= seekPts) {
                videoDecode->notify(data);
                break;
            }

        }
    }
    //  mutex.unlock();
    LOGS("seek debug:Seek end");
    //seek之后重新开启所有的线程
    SetPause(false);
    return re;
}

void IPlayer::SetPause(bool isP) {
    //   mutex.lock();
    const std::lock_guard<std::mutex> lock(mutex);

    XThread::SetPause(isP);
    if (iDemux) {
        iDemux->SetPause(isP);
    }
    if (videoDecode) {
        videoDecode->SetPause(isP);
    }
    if (audioDecode) {
        audioDecode->SetPause(isP);
    }
    if (audioPlay) {
        LOGS("audioPlay->SetPause:");
        audioPlay->SetPause(isP);
    }
    //  mutex.unlock();
}


