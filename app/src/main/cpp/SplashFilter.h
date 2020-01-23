//
// Created by yanyinan on 2020/1/22.
//

#ifndef MANCHESTERUNITEDPLAYER_SPLASHFILTER_H
#define MANCHESTERUNITEDPLAYER_SPLASHFILTER_H


#include "Filter.h"
/**
 * 闪白效果
 */
class SplashFilter: public Filter {
    virtual void onDraw(int i);

    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onShaderDataLoad(unsigned int program);
};


#endif //MANCHESTERUNITEDPLAYER_SPLASHFILTER_H
