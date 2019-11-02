//
// Created by yanxi on 2019/10/19.
//

#ifndef MANCHESTERUNITEDPLAYER_FFRESAMPLE_H
#define MANCHESTERUNITEDPLAYER_FFRESAMPLE_H


#include "IResample.h"
//todo 这样子定义的目的是？
struct SwrContext;
class FFResample : public IResample{
public:
    virtual bool Open(XParameter in, XParameter out = XParameter());
    virtual void Close();
    virtual XData Resample(XData xData);
protected:
    SwrContext *swrContext = 0;
    std::mutex mutex1;
};


#endif //MANCHESTERUNITEDPLAYER_FFRESAMPLE_H
