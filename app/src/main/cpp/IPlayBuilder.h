//
// Created by yanxi on 2019/10/28.
//

#ifndef MANCHESTERUNITEDPLAYER_IPLAYBUILDER_H
#define MANCHESTERUNITEDPLAYER_IPLAYBUILDER_H


#include "IPlayer.h"

class IPlayBuilder {
public:
    virtual IPlayer *BuildPlayer(unsigned int index = 0);

protected:
    virtual IDemux *CreateDemux() = 0;

    virtual IDecode *CreateDecode() = 0;

    virtual IResample *CreateResample() = 0;

    virtual IVideoView *CreateVideoView() = 0;

    virtual IAudioPlay *CreateAudioPlay() = 0;

    virtual IPlayer *CreatePalyer(unsigned int index) = 0;

};


#endif //MANCHESTERUNITEDPLAYER_IPLAYBUILDER_H
