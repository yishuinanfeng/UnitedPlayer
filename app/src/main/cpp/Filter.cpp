//
// Created by yanyinan on 2020/1/20.
//

#include "Filter.h"
#include "XLog.h"
#include "ShaderHandler.h"

GLuint Filter::getFragmentShader(int shaderType) {
    switch (shaderType) {
        case SHADER_YUV420P:
            getShaderForYuv420p();
            break;
        case SHADER_NV12:
            getShaderForNV12();
            LOGDSHADER("getShaderForNV12 fsh:%d",fsh);
            break;
        case SHADER_NV21:
            getShaderForNV21();
            break;
        default:
            LOGDSHADER("YuvType is error");
            return 0;
    }
    if (fsh == 0) {
        LOGDSHADER("ShaderHandler initShader GL_FRAGMENT_SHADER failed");
        return 0;
    }
    LOGDSHADER("getFragmentShader fsh:%d",fsh);
    return fsh;
}

GLuint Filter::initFragShader(const char *source) {
    //创建shader
    GLuint sh = glCreateShader(GL_FRAGMENT_SHADER);
    if (sh == 0) {
        LOGDT("glCreateShader fragment failed");
        return 0;
    }
    LOGDT("loadTexture initShader:%s:", source);
    //加载shader
    glShaderSource(sh,
                   1,//shader数量
                   &source,
                   nullptr);//代码长度，传0则读到字符串结尾

    //编译shader
    glCompileShader(sh);

    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        int logLength;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logLength);
        char *logBuffer = new char[logLength];
        glGetShaderInfoLog(sh, logLength, NULL, logBuffer);
        LOGDSHADER("glCompileShader  failed,log:%s", logBuffer);
        LOGDSHADER("source %s", source);
        return 0;
    }

    LOGDSHADER("glCompileShader success");
    return sh;
}

void Filter::getShaderForYuv420p() {
    fsh = initFragShader(getSpecifiedFragShader(SHADER_YUV420P));
}

void Filter::getShaderForNV21() {
    fsh = initFragShader(getSpecifiedFragShader(SHADER_NV21));
}

void Filter::getShaderForNV12() {
    fsh = initFragShader(getSpecifiedFragShader(SHADER_NV12));

}




