//
// Created by yanyinan on 2020/1/21.
//

#include "NoneFilter.h"
#include "FragmentShader.h"
#include "ShaderHandler.h"
#include "XLog.h"

void NoneFilter::onDraw(int pts) {
    //绘制矩形图像
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
