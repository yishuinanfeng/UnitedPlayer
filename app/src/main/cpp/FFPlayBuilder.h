//
// Created by yanxi on 2019/10/28.
//

#ifndef MANCHESTERUNITEDPLAYER_FFPLAYBUILDER_H
#define MANCHESTERUNITEDPLAYER_FFPLAYBUILDER_H


#include "IPlayer.h"
#include "IPlayBuilder.h"

class FFPlayBuilder :public IPlayBuilder{
public:
    static void InitHard(void *vm);

    static FFPlayBuilder *Get() {
        static FFPlayBuilder ffPlayBuilder;
        return &ffPlayBuilder;
    }

protected:
    FFPlayBuilder() {}

    virtual IDemux *CreateDemux();

    virtual IDecode *CreateDecode();

    virtual IResample *CreateResample();

    virtual IVideoView *CreateVideoView();

    virtual IAudioPlay *CreateAudioPlay();

    virtual IPlayer *CreatePalyer(unsigned int index);

};


#endif //MANCHESTERUNITEDPLAYER_FFPLAYBUILDER_H
