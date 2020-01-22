//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_SHADERHANDLER_H
#define MANCHESTERUNITEDPLAYER_SHADERHANDLER_H

#include <mutex>
#include "Filter.h"

//为了使和XTexture不关联
enum YuvType {
    SHADER_YUV420P = 0,//每4个像素Y4 U1 V1
    SHADER_NV12 = 25, //每4个像素Y4 UV1
    SHADER_NV21 = 26 //每4个像素Y4 VU1
};
/**
 * 滤镜类型
 */
enum FilterType {
    OPPOSITE_COLOR = 1,//反色
    GRAY = 2, //灰度图
    SPLASH = 3, //闪白
    SCALE = 4, //闪白
};
/**
 * 着色器处理器
 */
class ShaderHandler {
public:
    virtual bool Init(YuvType yuvType, int i);

    virtual void Close();

    //初始化绑定纹理  isA表示是否有透明通道（NV12,NV21）
    virtual void
    loadTexture(unsigned int index, int width, int height, unsigned char *buf, bool isAlpha = false);

    virtual void Draw(int i);

    //着色器Id
    unsigned int vsh = 0;
    GLuint fsh = 0;
    //着色器程序id
    unsigned int program = 0;
    //100层的纹理id数组
    unsigned int textures[100] = {0};
    std::mutex mutex;
    //todo 动态滤镜的时间参数
    int uTimeLocation = 0;
    int uScaleMatrixLocation;
    int filterType = -1;

private:
    Filter *filter = nullptr;
};


#endif //MANCHESTERUNITEDPLAYER_SHADERHANDLER_H
