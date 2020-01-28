//
// Created by yanyinan on 2020/1/22.
//

#ifndef MANCHESTERUNITEDPLAYER_SOULFILTER_H
#define MANCHESTERUNITEDPLAYER_SOULFILTER_H


#include "Filter.h"

#define SCALE_DURATION  1000
#define MAX_DIFF_SCALE  1.0F
#define SKIP_DURATION  150

using namespace glm;
/**
 * 灵魂出窍效果
 */
class SoulFilter : public Filter{
    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onDraw(int i);

    virtual void onShaderDataLoad(unsigned int i);

private:
    int uScaleLocation;
    int uTimeLocation;
    int uAlphaLocation = 0;
    int uFrontAlphaLocation = 0;

};


#endif //MANCHESTERUNITEDPLAYER_SOULFILTER_H
