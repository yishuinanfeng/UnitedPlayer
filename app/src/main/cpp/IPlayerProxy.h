//
// Created by yanxi on 2019/10/29.
//

#ifndef MANCHESTERUNITEDPLAYER_IPLAYERPROXY_H
#define MANCHESTERUNITEDPLAYER_IPLAYERPROXY_H


#include <mutex>
#include "IPlayer.h"

class IPlayerProxy : public IPlayer {
public:
    static IPlayerProxy* Get();

    void Init(void* vm = 0);

    virtual bool Open(const char *path);

    virtual bool Start();

    virtual bool InitView(void *win);

protected:
    IPlayerProxy();

    IPlayer *player = 0;
    std::mutex mutex;

};


#endif //MANCHESTERUNITEDPLAYER_IPLAYERPROXY_H
