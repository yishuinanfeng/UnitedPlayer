//
// Created by yanxi on 2019/10/28.
//

#include "FFPlayBuilder.h"
#include "FFDemux.h"
#include "FFDecode.h"
#include "FFResample.h"
#include "GLVideoView.h"
#include "SLAudioPlay.h"

IDemux *FFPlayBuilder::CreateDemux() {
    return new FFDemux();
}

IDecode *FFPlayBuilder::CreateDecode() {
    return new FFDecode();
}

IResample *FFPlayBuilder::CreateResample() {
    return new FFResample();
}

IVideoView *FFPlayBuilder::CreateVideoView() {
    return new GLVideoView();
}

IAudioPlay *FFPlayBuilder::CreateAudioPlay() {
    return new SLAudioPlay();
}

IPlayer *FFPlayBuilder::CreatePalyer(unsigned int index) {
    return IPlayer::Get(index);
}

void FFPlayBuilder::InitHard(void *vm) {
    FFDecode::InitHard(vm);
}
