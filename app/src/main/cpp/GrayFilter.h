//
// Created by yanyinan on 2020/1/21.
//

#ifndef MANCHESTERUNITEDPLAYER_GRAYFILTER_H
#define MANCHESTERUNITEDPLAYER_GRAYFILTER_H


#include "ShaderHandler.h"
#include "Filter.h"
/**
 * 灰度滤镜
 */
class GrayFilter: public Filter {

    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onDraw(int i);

    virtual void onShaderDataLoad(unsigned int i);
};


#endif //MANCHESTERUNITEDPLAYER_GRAYFILTER_H
