//
// Created by yanxi on 2019/10/20.
//

#ifndef MANCHESTERUNITEDPLAYER_SLAUDIOPLAY_H
#define MANCHESTERUNITEDPLAYER_SLAUDIOPLAY_H


#include <list>
#include "IAudioPlay.h"

class SLAudioPlay: public IAudioPlay {
public:
    virtual bool StartPlay(XParameter out);
    virtual void PlayCall(void *bufq);

    SLAudioPlay();
    virtual ~SLAudioPlay();

protected:
    unsigned char *buf = 0;
};


#endif //MANCHESTERUNITEDPLAYER_SLAUDIOPLAY_H
