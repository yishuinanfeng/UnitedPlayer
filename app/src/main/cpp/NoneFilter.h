//
// Created by yanyinan on 2020/1/21.
//

#ifndef MANCHESTERUNITEDPLAYER_NONEFILTER_H
#define MANCHESTERUNITEDPLAYER_NONEFILTER_H


#include "Filter.h"
/**
 * 不带滤镜效果
 */
class NoneFilter : public Filter {

    virtual void onDraw();

    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onShaderDataLoad();
};


#endif //MANCHESTERUNITEDPLAYER_NONEFILTER_H
