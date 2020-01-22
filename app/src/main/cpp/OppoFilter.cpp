//
// Created by yanyinan on 2020/1/21.
//

#include "OppoFilter.h"
#include "FragmentShader.h"
#include "ShaderHandler.h"

const char *OppoFilter::getSpecifiedFragShader(int yuvType) {
    switch (yuvType) {
        case SHADER_YUV420P:
        default:
            return fragYUV420POppositeColor;
        case SHADER_NV12:
            return fragNV12OppoColor;
        case SHADER_NV21:
            return fragNV21OppoColor;
    }

}

void OppoFilter::onDraw(int i) {

}

void OppoFilter::onShaderDataLoad(unsigned int i) {

}


