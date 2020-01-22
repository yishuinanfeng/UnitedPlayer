//
// Created by yanxi on 2019/9/15.
//

#include "ShaderHandler.h"
#include "XLog.h"
#include "IPlayer.h"
#include "FragmentShader.h"
#include "OppoFilter.h"
#include "GrayFilter.h"
#include "NoneFilter.h"
#include "SplashFilter.h"
#include <GLES2/gl2.h>
#include <ctime>

GLuint initShader(const char *source, int type);

GLuint initShader(const char *source, GLint type) {
    //创建shader
    GLuint sh = glCreateShader(type);
    if (sh == 0) {
        LOGDSHADER("glCreateShader %d failed", type);
        return 0;
    }
    LOGDSHADER("loadTexture initShader:%s:", source);
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
        int logLength;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logLength);
        char *logBuffer = new char[logLength];
        glGetShaderInfoLog(sh, logLength, NULL, logBuffer);
        LOGDSHADER("glCompileShader %d failed,log:%s", type, logBuffer);
        LOGDSHADER("source %s", source);
        return 0;
    }

    LOGDSHADER("glCompileShader %d success", type);
    return sh;
}

bool ShaderHandler::Init(YuvType yuvType, int filterType) {
    Close();
    this->filterType = filterType;
    const std::lock_guard<std::mutex> lock(mutex);
    vsh = initShader(vertexShader, GL_VERTEX_SHADER);
    if (vsh == 0) {
        LOGDSHADER("ShaderHandler initShader GL_VERTEX_SHADER failed");
        return false;
    }
    LOGDSHADER("ShaderHandler initShader GL_VERTEX_SHADER success");

    switch (filterType) {
        case OPPOSITE_COLOR:
            filter = new OppoFilter();
            LOGDSHADER("OppoFilter");
            break;
        case GRAY:
            filter = new GrayFilter();
            LOGDSHADER("GrayFilter");
            break;
        case SPLASH:
            filter = new SplashFilter();
            LOGDSHADER("SplashFilter");
            break;
        default:
            filter = new NoneFilter();
            LOGDSHADER("NoneFilter");
            break;
    }

    LOGDSHADER("getFragmentShader");

    fsh = filter->getFragmentShader(yuvType);

    if (fsh == 0) {
        LOGDSHADER("ShaderHandler initShader GL_FRAGMENT_SHADER failed");
        return false;
    }
    LOGDSHADER("ShaderHandler initShader GL_FRAGMENT_SHADER success");

    //创建渲染程序
    program = glCreateProgram();
    if (program == 0) {
        LOGDSHADER("ShaderHandler glCreateProgram failed");
        return false;
    }

    //向渲染程序中加入着色器
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);

    //链接程序
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        LOGDT("ShaderHandler glLinkProgram failed");
        return false;
    }
    LOGDT("ShaderHandler glLinkProgram success");
    //激活渲染程序
    glUseProgram(program);

    //加入三维顶点数据
    static float ver[] = {
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };

    GLuint apos = static_cast<GLuint>(glGetAttribLocation(program, "aPosition"));
    glEnableVertexAttribArray(apos);
    glVertexAttribPointer(apos, 3, GL_FLOAT, GL_FALSE, 12, ver);

    //加入纹理坐标数据
    static float fragment[] = {
            1.0f, 0.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
    };
    GLuint aTex = static_cast<GLuint>(glGetAttribLocation(program, "aTextCoord"));
    glEnableVertexAttribArray(aTex);
    glVertexAttribPointer(aTex, 2, GL_FLOAT, GL_FALSE, 8, fragment);

    uTimeLocation = glGetUniformLocation(program, "u_time");
    filter->onShaderDataLoad();

    //纹理初始化
    //设置纹理层
    //todo 这里是指定各个sampler2D变量对应哪一层纹理？
    glUniform1i(glGetUniformLocation(program, "yTexture"), 0);
    switch (yuvType) {
        case SHADER_YUV420P:
            glUniform1i(glGetUniformLocation(program, "uTexture"), 1);
            glUniform1i(glGetUniformLocation(program, "vTexture"), 2);
            break;
            //NV12,NV21只需要两层纹理
        case SHADER_NV12:
        case SHADER_NV21:
            glUniform1i(glGetUniformLocation(program, "uvTexture"), 1);
            break;

    }

    LOGDT("ShaderHandler 初始化Shader成功");
    return true;
}

/**
 * 获取纹理并映射到内存中
 * @param index
 * @param width
 * @param height
 * @param buf
 */
void ShaderHandler::loadTexture(unsigned int index, int width, int height, unsigned char *buf, bool isAlpha) {
    // LOGD("ShaderHandler loadTexture width:%d,,height:%d,buf:%x" ,width ,height,buf);
    //gpu内部格式 亮度，灰度图（这里就是按照亮度值存储纹理单元 ，只取一个亮度的颜色通道的意思），格式默认为灰度
    unsigned int format = GL_LUMINANCE;
    if (isAlpha) {
        //带透明通道，按照亮度和alpha值存储纹理单元
        format = GL_LUMINANCE_ALPHA;
    }

    const std::lock_guard<std::mutex> lock(mutex);

    if (textures[index] == 0) {
        glGenTextures(1, &textures[index]);
        //LOGD("ShaderHandler loadTexture texture id:%d:",text[index]);
        //绑定纹理。后面的的设置和加载全部作用于当前绑定的纹理对象
        //GL_TEXTURE0、GL_TEXTURE1、GL_TEXTURE2 的就是纹理单元，GL_TEXTURE_1D、GL_TEXTURE_2D、CUBE_MAP为纹理目标
        //通过 glBindTexture 函数将纹理目标和纹理绑定后，对纹理目标所进行的操作都反映到对纹理上
        glBindTexture(GL_TEXTURE_2D, textures[index]);
        //缩小的过滤器
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //放大的过滤器
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //设置纹理的格式和大小
        // 加载纹理到 OpenGL，读入 buffer 定义的位图数据，并把它复制到当前绑定的纹理对象
        // 当前绑定的纹理对象就会被附加上纹理图像。
        //width,height表示每几个像素公用一个yuv元素？比如width / 2表示横向每两个像素使用一个元素？
        glTexImage2D(GL_TEXTURE_2D,
                     0,//细节基本 默认0
                     format,//gpu内部格式 亮度，灰度图（这里就是只取一个亮度的颜色通道的意思）
                     width,//加载的纹理宽度。最好为2的次幂
                     height,//加载的纹理高度。最好为2的次幂
                     0,//纹理边框
                     format,//数据的像素格式 亮度，灰度图
                     GL_UNSIGNED_BYTE,//像素点存储的数据类型
                     nullptr //纹理的数据（先不传）
        );
    }
    //  LOGD("ShaderHandler glActiveTexture");
    //激活第一层纹理，绑定到创建的纹理
    //下面的width,height主要是显示尺寸？
    glActiveTexture(GL_TEXTURE0 + index);
    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    //替换纹理，比重新使用glTexImage2D性能高多
    LOGDT("loadTexture width:%d, height:%d", width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0,//相对原来的纹理的offset
                    width, height,//加载的纹理宽度、高度。最好为2的次幂
                    format, GL_UNSIGNED_BYTE,
                    buf);

}

void ShaderHandler::Draw(int pts) {

    const std::lock_guard<std::mutex> lock(mutex);

    if (!program) {
        return;
    }
    LOGE("xShader Draw");

    glUniform1f(uTimeLocation, pts);

    filter->onDraw();

    //绘制矩形图像
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


}

void ShaderHandler::Close() {
    const std::lock_guard<std::mutex> lock(mutex);

    //要先释放program，如果先释放shader的话程序还会访问shader，导致出错？
    if (program) {
        glDeleteProgram(program);
    }
    if (fsh) {
        glDeleteShader(fsh);
    }
    if (vsh) {
        glDeleteShader(vsh);
    }

    for (int i = 0; i < sizeof(textures) / sizeof(unsigned int); ++i) {
        if (textures[i]) {
            glDeleteTextures(1, &textures[i]);
        }
        textures[i] = 0;
    }

    delete filter;

}

