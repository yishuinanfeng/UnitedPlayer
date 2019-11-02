//
// Created by yanxi on 2019/10/20.
//

#ifndef MANCHESTERUNITEDPLAYER_IAUDIOPLAY_H
#define MANCHESTERUNITEDPLAYER_IAUDIOPLAY_H


#include <list>
#include "IObserver.h"
#include "XParameter.h"

class IAudioPlay : public IObserver{
public:
    //缓冲满后阻塞
    virtual void Update(XData xData);
    virtual bool StartPlay(XParameter out) = 0;
    //获取缓冲数据，如没有则阻塞
    virtual XData GetData();
    int maxFrame = 100;
    int pst = 0;
protected:
    std::list <XData> framelist;
    std::mutex frameMutex;
};


#endif //MANCHESTERUNITEDPLAYER_IAUDIOPLAY_H
