//
// Created by yanyinan on 2020/1/22.
//

#include <cmath>
#include <glm/ext.hpp>
#include "SoulFilter.h"
#include "ShaderHandler.h"
#include "FragmentShader.h"
#include "XLog.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/detail/_noise.hpp"
#include "glm/gtx/transform.hpp"

const char *SoulFilter::getSpecifiedFragShader(int yuvType) {
    switch (yuvType) {
        case SHADER_YUV420P:
        default:
            return fragYUV420PSoul;
        case SHADER_NV12:
            return fragNV12Soul;
        case SHADER_NV21:
            return fragNV21Soul;
    }
}

void SoulFilter::onDraw(int pts) {
//存在两个图层,开启颜色混合
    //glEnable(GL_BLEND);
    //透明度混合
    // 表示源颜色乘以自身的alpha 值，目标颜色乘目标颜色值混合，如果不开启，直接被目标的画面覆盖
    //   glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);

    int remainder = pts % SCALE_DURATION;
    LOGDSHADER("SoulFilter onDraw remainder:%d", remainder);
    float ratio;
    float frontAlpha;
    float backAlpha;
    //算出pts对scaleTime区取余的余数占scaleTime多少
    if (remainder < SKIP_DURATION) {
        ratio = 0;
        frontAlpha = 1;
        backAlpha = 1;
    } else {
        ratio = remainder * 1.0F / SCALE_DURATION;
        frontAlpha = ratio;
        backAlpha = 1 - frontAlpha;
    }

    //最大缩放倍数为MAX_SCALE
    float scale = 1.0F + MAX_DIFF_SCALE * ratio;
    LOGDSHADER("SoulFilter onDraw scale:%f", scale);
    //绘制顶层图像
   // glUniform1f(uAlphaLocation, backAlpha);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//    mat4 scaleMatrix;
//    mat4 resultMatrix = glm::scale(scaleMatrix, vec3(scale));
//    glUniformMatrix4fv(uScaleMatrixLocation, 1, GL_FALSE, glm::value_ptr(resultMatrix));
    glUniform1f(uScaleLocation, scale);
    //绘制底层图像
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


   // glDisable(GL_BLEND);
}

void SoulFilter::onShaderDataLoad(unsigned int program) {
    uTimeLocation = glGetUniformLocation(program, "u_time");
    uScaleLocation = glGetUniformLocation(program, "uScale");
    uAlphaLocation = glGetUniformLocation(program, "uAlpha");
}
