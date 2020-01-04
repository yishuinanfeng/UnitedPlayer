//
// Created by yanxi on 2019/10/29.
//

#ifndef MANCHESTERUNITEDPLAYER_IPLAYERPROXY_H
#define MANCHESTERUNITEDPLAYER_IPLAYERPROXY_H


#include <mutex>
#include "IPlayer.h"

class IPlayerProxy : public IPlayer {
public:
    static IPlayerProxy *Get();

    void Init(void *vm = 0);

    virtual bool Open(const char *path);

    virtual void Close();

    virtual bool Start();

    virtual bool InitView(void *win, int filterType);

    virtual double GetPlayPose();

    virtual bool Seek(double position);

    virtual void SetPause(bool isP);

    virtual bool IsPause();


protected:
    IPlayerProxy();

    IPlayer *player = 0;
    std::mutex mutexProxy;

};


#endif //MANCHESTERUNITEDPLAYER_IPLAYERPROXY_H
