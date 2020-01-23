//
// Created by yanyinan on 2020/1/23.
//

#ifndef MANCHESTERUNITEDPLAYER_SHAKEFILTER_H
#define MANCHESTERUNITEDPLAYER_SHAKEFILTER_H


#include "Filter.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/ext.hpp"
#include "glm/glm/detail/_noise.hpp"
#include "glm/gtx/transform.hpp"

#define SCALE_DURATION  600
#define SKIP_DURATION  150
#define MAX_DIFF_SCALE  0.2F

using namespace glm;
/**
 * 抖动效果
 */
class ShakeFilter : public Filter {

    virtual void onDraw(int pts);

    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onShaderDataLoad(unsigned int program);

private:
    int uScaleMatrixLocation;
    int uTextureCoordOffsetLocation;

};


#endif //MANCHESTERUNITEDPLAYER_SHAKEFILTER_H
