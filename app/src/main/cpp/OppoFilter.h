//
// Created by yanyinan on 2020/1/21.
//

#ifndef MANCHESTERUNITEDPLAYER_OPPOFILTER_H
#define MANCHESTERUNITEDPLAYER_OPPOFILTER_H

#include "Filter.h"

/**
 * 反色滤镜
 */
class OppoFilter : public Filter {

    virtual const char *getSpecifiedFragShader(int yuvType);

    virtual void onDraw(int i);

    virtual void onShaderDataLoad(unsigned int i);

};


#endif //MANCHESTERUNITEDPLAYER_OPPOFILTER_H
