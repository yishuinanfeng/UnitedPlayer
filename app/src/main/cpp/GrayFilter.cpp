//
// Created by yanyinan on 2020/1/21.
//

#include "GrayFilter.h"
#include "FragmentShader.h"

const char *GrayFilter::getSpecifiedFragShader(int yuvType) {
    switch (yuvType) {
        case SHADER_YUV420P:
        default:
            return fragYUV420PGray;
        case SHADER_NV12:
            return fragNV12Gray;
        case SHADER_NV21:
            return fragNV21Gray;
    }
}

void GrayFilter::onDraw(int pts) {
    //绘制矩形图像
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GrayFilter::onShaderDataLoad(unsigned int i) {

}
