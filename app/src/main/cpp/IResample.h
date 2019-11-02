//
// Created by yanxi on 2019/10/19.
//

#ifndef MANCHESTERUNITEDPLAYER_IRESAMPLE_H
#define MANCHESTERUNITEDPLAYER_IRESAMPLE_H


#include "IObserver.h"
#include "XParameter.h"

class IResample : public IObserver {
public:
    int outChannels = 2;
    int outFormat = 1;

    virtual bool Open(XParameter in, XParameter out = XParameter()) = 0;

    virtual void Close() = 0;

    virtual XData Resample(XData inData) = 0;

    virtual void Update(XData data);
};


#endif //MANCHESTERUNITEDPLAYER_IRESAMPLE_H
