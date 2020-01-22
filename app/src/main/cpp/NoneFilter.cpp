//
// Created by yanyinan on 2020/1/21.
//

#include "NoneFilter.h"
#include "FragmentShader.h"
#include "ShaderHandler.h"
#include "XLog.h"

void NoneFilter::onDraw(int i) {

}

const char *NoneFilter::getSpecifiedFragShader(int yuvType) {
    switch (yuvType) {
        case SHADER_YUV420P:
        default:
            return fragYUV420P;
        case SHADER_NV12:
            LOGDSHADER("getSpecifiedFragShader SHADER_NV12");
            return fragNV12;
        case SHADER_NV21:
            return fragNV21;
    }
}

void NoneFilter::onShaderDataLoad(unsigned int i) {

}
