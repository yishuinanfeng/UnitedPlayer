//
// Created by yanyinan on 2020/1/22.
//

#ifndef MANCHESTERUNITEDPLAYER_SOULFILTER_H
#define MANCHESTERUNITEDPLAYER_SOULFILTER_H


#include "Filter.h"

#define SCALE_DURATION  600
#define MAX_DIFF_SCALE  3.0F

using namespace glm;
/**
 * 灵魂出窍效果
 */
class SoulFilter : public Filter{
    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onDraw(int i);

    virtual void onShaderDataLoad(unsigned int i);

private:
    int uScaleMatrixLocation;
    int uTimeLocation;
    int uAlphaLocation = 0;
    int uFrontAlphaLocation = 0;

};


#endif //MANCHESTERUNITEDPLAYER_SOULFILTER_H
