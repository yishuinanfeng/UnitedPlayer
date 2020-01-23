//
// Created by yanyinan on 2020/1/22.
//

#include "ScaleFilter.h"
#include "ShaderHandler.h"
#include "FragmentShader.h"
#include "XLog.h"

const char *ScaleFilter::getSpecifiedFragShader(int yuvType) {
    switch (yuvType) {
        case SHADER_YUV420P:
        default:
            return fragYUV420PScale;
        case SHADER_NV12:
            return fragNV12Scale;
        case SHADER_NV21:
            return fragNV21Scale;
    }
}

void ScaleFilter::onDraw(int pts) {
    //一个缩放动画的持续时间
    int scaleDuration = SCALE_DURATION;
    int remainder = pts % scaleDuration;
    LOGDSHADER("ScaleFilter onDraw remainder:%d", remainder);
    float ratio;
    //算出pts对scaleTime区取余的余数占scaleTime多少
    if (remainder < SKIP_DURATION){
        ratio = 1;
    }
    else if (remainder > scaleDuration / 2) {
        ratio = remainder * 1.0F / scaleDuration;
    } else {
        //缩小速度加速度增快
        ratio = static_cast<float>(pow((1.0F - remainder * 1.0F / scaleDuration), 2));
      //  ratio = (1.0F - remainder * 1.0F / scaleDuration);
    }

    //最大缩放倍数为3.0F
    float scale = MAX_DIFF_SCALE * ratio;
    if (scale <1){
        scale = 1;
    }
    LOGDSHADER("ScaleFilter onDraw scale:%f", scale);
    mat4 scaleMatrix;
    mat4 resultMatrix = glm::scale(scaleMatrix, vec3(scale));
    glUniformMatrix4fv(uScaleMatrixLocation, 1, GL_FALSE, glm::value_ptr(resultMatrix));

    //绘制矩形图像
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ScaleFilter::onShaderDataLoad(unsigned int program) {
    uTimeLocation = glGetUniformLocation(program, "u_time");
    uScaleMatrixLocation = glGetUniformLocation(program, "uScaleMatrix");

}
