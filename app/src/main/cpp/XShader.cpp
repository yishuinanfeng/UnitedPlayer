//
// Created by yanxi on 2019/9/15.
//

#include "XShader.h"
#include "XLog.h"
#include <GLES2/gl2.h>
#include <ctime>

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

static const char *fragNV12 = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

static const char *fragNV21 = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
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
        LOGDT("glCreateShader %d failed", type);
        return 0;
    }
    LOGDT("GetTexture initShader:%s:", source);
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
        LOGDT("glCompileShader %d failed", type);
        LOGDT("source %s", source);
        return 0;
    }

    LOGDT("glCompileShader %d success", type);
    return sh;
}

bool XShader::Init(XShaderType shaderType) {
    vsh = initShader(vertexShader, GL_VERTEX_SHADER);
    if (vsh == 0) {
        LOGDT("XShader initShader GL_VERTEX_SHADER failed");
        return false;
    }
    LOGDT("XShader initShader GL_VERTEX_SHADER success");
    switch (shaderType) {
        case XSHDER_YUV420P:
            fsh = initShader(fragYUV420P, GL_FRAGMENT_SHADER);
            break;
        case XSHDER_NV12:
            fsh = initShader(fragNV12, GL_FRAGMENT_SHADER);
            break;
        case XSHDER_NV21:
            fsh = initShader(fragNV12, GL_FRAGMENT_SHADER);
            break;
        default:
            LOGDT("XShaderType is error");
            return false;
    }
    if (fsh == 0) {
        LOGDT("XShader initShader GL_FRAGMENT_SHADER failed");
        return false;
    }
    LOGDT("XShader initShader GL_FRAGMENT_SHADER success");

    //创建渲染程序
    program = glCreateProgram();
    if (program == 0) {
        LOGDT("XShader glCreateProgram failed");
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
        LOGDT("XShader glLinkProgram failed");
        return false;
    }
    LOGDT("XShader glLinkProgram success");
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

    //纹理初始化
    //设置纹理层

    glUniform1i(glGetUniformLocation(program, "yTexture"), 0);
    switch (shaderType) {
        case XSHDER_YUV420P:
            glUniform1i(glGetUniformLocation(program, "uTexture"), 1);
            glUniform1i(glGetUniformLocation(program, "vTexture"), 2);
            break;
        case XSHDER_NV12:
        case XSHDER_NV21:
            //NV12,NV21只需要两层纹理（因为数据只有数组？）
            glUniform1i(glGetUniformLocation(program, "uvTexture"), 1);
            break;

    }

    LOGDT("XShader 初始化Shader成功");
    return true;
}

/**
 * 获取纹理并映射到内存中
 * @param index
 * @param width
 * @param height
 * @param buf
 */
void XShader::GetTexture(unsigned int index, int width, int height, unsigned char *buf, bool isA) {
    // LOGD("XShader GetTexture width:%d,,height:%d,buf:%x" ,width ,height,buf);
    //gpu内部格式 亮度，灰度图（这里就是按照亮度值存储纹理单元 ，只取一个亮度的颜色通道的意思），格式默认为灰度
    unsigned int format = GL_LUMINANCE;
    if (isA) {
        //带透明通道，按照亮度和alpha值存储纹理单元
        format = GL_LUMINANCE_ALPHA;
    }
    if (text[index] == 0) {
        glGenTextures(1, &text[index]);
        //LOGD("XShader GetTexture texture id:%d:",text[index]);
        //绑定纹理。后面的的设置和加载全部作用于当前绑定的纹理对象
        //GL_TEXTURE0、GL_TEXTURE1、GL_TEXTURE2 的就是纹理单元，GL_TEXTURE_1D、GL_TEXTURE_2D、CUBE_MAP为纹理目标
        //通过 glBindTexture 函数将纹理目标和纹理绑定后，对纹理目标所进行的操作都反映到对纹理上
        glBindTexture(GL_TEXTURE_2D, text[index]);
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
    //  LOGD("XShader glActiveTexture");
    //激活第一层纹理，绑定到创建的纹理
    //下面的width,height主要是显示尺寸？
    glActiveTexture(GL_TEXTURE0 + index);
    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, text[index]);
    //替换纹理，比重新使用glTexImage2D性能高多
    LOGDT("GetTexture width:%d, height:%d", width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0,//相对原来的纹理的offset
                    width, height,//加载的纹理宽度、高度。最好为2的次幂
                    format, GL_UNSIGNED_BYTE,
                    buf);
}

void XShader::Draw() {
    if (!program) {
        return;
    }
    LOGE("xShader Draw");
    //绘制矩形图像
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
