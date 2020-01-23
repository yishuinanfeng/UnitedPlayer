//
// Created by yanyinan on 2020/1/22.
//

#ifndef MANCHESTERUNITEDPLAYER_SCALEFILTER_H
#define MANCHESTERUNITEDPLAYER_SCALEFILTER_H


#include "Filter.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/ext.hpp"
#include "glm/glm/detail/_noise.hpp"
#include "glm/gtx/transform.hpp"

using namespace glm;

#define SCALE_DURATION  600
#define SKIP_DURATION  100
#define MAX_DIFF_SCALE  3.0F

class ScaleFilter : public Filter {
    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onDraw(int i);

    virtual void onShaderDataLoad(unsigned int i);

private:
    int uScaleMatrixLocation;
    int uTimeLocation;
};


#endif //MANCHESTERUNITEDPLAYER_SCALEFILTER_H
