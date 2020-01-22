//
// Created by yanyinan on 2020/1/22.
//

#ifndef MANCHESTERUNITEDPLAYER_SPLASHFILTER_H
#define MANCHESTERUNITEDPLAYER_SPLASHFILTER_H


#include "Filter.h"

class SplashFilter: public Filter {
    virtual void onDraw();

    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onShaderDataLoad();
};


#endif //MANCHESTERUNITEDPLAYER_SPLASHFILTER_H
