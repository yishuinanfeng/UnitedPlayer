//
// Created by yanyinan on 2020/1/23.
//

#include "ShakeFilter.h"
#include "ShaderHandler.h"
#include "FragmentShader.h"
#include "XLog.h"

void ShakeFilter::onDraw(int pts) {
    //一个缩放动画的持续时间
    int scaleDuration = SCALE_DURATION;
    int remainder = pts % scaleDuration;
    LOGDSHADER("ScaleFilter onDraw remainder:%d", remainder);
    float ratio;
    //算出pts对scaleTime区取余的余数占scaleTime多少
    if (remainder < SKIP_DURATION) {
        ratio = 0;
    } else {
        ratio = remainder * 1.0F / scaleDuration;
    }

    //最大缩放倍数为MAX_SCALE
    float scale = 1.0F + MAX_DIFF_SCALE * ratio;

    LOGDSHADER("ScaleFilter onDraw scale:%f", scale);
    mat4 scaleMatrix;
    mat4 resultMatrix = glm::scale(scaleMatrix, vec3(scale));
    glUniformMatrix4fv(uScaleMatrixLocation, 1, GL_FALSE, glm::value_ptr(resultMatrix));
    //纹理坐标的偏移量只需要一个很小的值（因为纹理坐标是归一化的）
    float textureCoordOffset = 0.005F * ratio;
    LOGDSHADER("ScaleFilter onDraw textureCoordOffset:%f", textureCoordOffset);
    glUniform1f(uTextureCoordOffsetLocation,textureCoordOffset);

    //绘制矩形图像
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

const char *ShakeFilter::getSpecifiedFragShader(int yuvType) {
    switch (yuvType) {
        case SHADER_YUV420P:
        default:
            return fragYUV420PShake;
        case SHADER_NV12:
            LOGDSHADER("getSpecifiedFragShader SHADER_NV12");
            return fragNV12Shake;
        case SHADER_NV21:
            return fragNV21Shake;
    }
}

void ShakeFilter::onShaderDataLoad(unsigned int program) {
    uScaleMatrixLocation = glGetUniformLocation(program, "uScaleMatrix");
    uTextureCoordOffsetLocation = glGetUniformLocation(program, "uTextureCoordOffset");
}
