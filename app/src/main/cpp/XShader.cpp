//
// Created by yanxi on 2019/9/15.
//

#include "XShader.h"
#include "XLog.h"
#include <GLES2/gl2.h>

//顶点着色器(static的意义)
#define GET_STR(x) #x
static const char *vertexShader = GET_STR(
        attribute
        vec4 aPosition;//输入的顶点坐标
        attribute
        vec2 aTextCoord;//输入的纹理坐标
        varying
        vec2 vTextCoord;//输出的纹理坐标
        void main() {
            //这里其实是将上下翻转过来（因为安卓图片会自动上下翻转，所以转回来）
            vTextCoord = vec2(aTextCoord.x, 1.0 - aTextCoord.y);
            gl_Position = aPosition;
        }
);

static const char *fragYUV420P = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

GLuint initShader(const char *source, int type);


GLuint initShader(const char *source, GLint type) {
    //创建shader
    GLuint sh = glCreateShader(type);
    if (sh == 0) {
        LOGE("glCreateShader %d failed", type);
        return 0;
    }
    //加载shader
    glShaderSource(sh,
                   1,//shader数量
                   &source,
                   0);//代码长度，传0则读到字符串结尾

    //编译shader
    glCompileShader(sh);

    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        LOGE("glCompileShader %d failed", type);
        LOGD("source %s", source);
        return 0;
    }

    LOGD("glCompileShader %d success", type);
    return sh;
}

bool XShader::Init() {
    vsh = initShader(vertexShader, GL_VERTEX_SHADER);
    if (vsh == 0) {
        LOGE("initShader GL_VERTEX_SHADER failed");
        return false;
    }
    LOGD("initShader GL_VERTEX_SHADER success");

    fsh = initShader(fragYUV420P, GL_FRAGMENT_SHADER);
    if (fsh == 0) {
        LOGE("initShader GL_FRAGMENT_SHADER failed");
        return false;
    }
    LOGD("initShader GL_FRAGMENT_SHADER success");

    //创建渲染程序
    program = glCreateProgram();
    if (program == 0) {
        LOGD("glCreateProgram failed");
        return false;
    }

    //向渲染程序中加入着色器
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);

    //链接程序
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == 0) {
        LOGD("glLinkProgram failed");
        return false;
    }
    LOGD("glLinkProgram success");
    //激活渲染程序
    glUseProgram(program);

    //纹理初始化
    //设置纹理层
    glUniform1i(glGetUniformLocation(program, "yTexture"), 0);
    glUniform1i(glGetUniformLocation(program, "uTexture"), 1);
    glUniform1i(glGetUniformLocation(program, "vTexture"), 2);

    LOGD("初始化Shader成功");
    return true;
}
