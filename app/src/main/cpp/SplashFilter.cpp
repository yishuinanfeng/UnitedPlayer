//
// Created by yanyinan on 2020/1/22.
//

#include "SplashFilter.h"
#include "ShaderHandler.h"
#include "FragmentShader.h"
#include "XLog.h"

void SplashFilter::onDraw(int pts) {
    //绘制矩形图像
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

const char *SplashFilter::getSpecifiedFragShader(int yuvType) {
    switch (yuvType) {
        case SHADER_YUV420P:
        default:
            return fragYUV420PSplash;
        case SHADER_NV12:
            LOGDSHADER("getSpecifiedFragShader SHADER_NV12");
            return fragNV12Splash;
        case SHADER_NV21:
            return fragNV21;
    }
}

void SplashFilter::onShaderDataLoad(unsigned int i) {

}
