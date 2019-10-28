//
// Created by yanxi on 2019/10/28.
//

#include "IPlayBuilder.h"
#include "IPlayer.h"
#include "IDemux.h"
#include "XLog.h"
#include "IDecode.h"
#include "IResample.h"
#include "IAudioPlay.h"
#include "IVideoView.h"

IPlayer *IPlayBuilder::BuildPlayer(unsigned int index) {
    IPlayer *play = CreatePalyer(index);
    IDemux *iDemux = CreateDemux();
    IDecode *audioDecode = CreateDecode();
    IDecode *videoDecode = CreateDecode();
    //解复用一帧之后，通知解码器
    iDemux->AddOberver(audioDecode);
    iDemux->AddOberver(videoDecode);
    IVideoView *iVideoView = CreateVideoView();
    //解码一帧之后，通知显示模块
    videoDecode->AddOberver(iVideoView);
    IResample *resample = CreateResample();
    audioDecode->AddOberver(resample);
    IAudioPlay *audioPlay = CreateAudioPlay();
    resample->AddOberver(audioPlay);

    play->iDemux = iDemux;
    play->audioPlay = audioPlay;
    play->audioDecode = audioDecode;
    play->videoDecode = videoDecode;
    play->resample = resample;
    play->videoView = iVideoView;
    return play;
}
