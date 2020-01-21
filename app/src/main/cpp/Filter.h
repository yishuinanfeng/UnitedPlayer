//
// Created by yanyinan on 2020/1/20.
//

#ifndef MANCHESTERUNITEDPLAYER_FILTER_H
#define MANCHESTERUNITEDPLAYER_FILTER_H


#include <GLES2/gl2.h>

class Filter {
public:
    /**
     * 供外部获取片段着色器
     * @param shaderType
     * @return
     */
    virtual GLuint getFragmentShader(int shaderType);


    /**
     * 绘制的时候当前滤镜需要处理的逻辑
     */
    virtual void onDraw() = 0;

    /**
     * 在纹理坐标加载之后（例如获取片段着色器的属性location）
     */
    virtual void onShaderDataLoad() = 0;

    /**
     * 得到具体的片段着色器代码（子类具体实现）
     * @return
     */
    virtual const char *getSpecifiedFragShader(int yuvType) = 0;

private:
    //片段着色器id
    GLuint fsh = 0;

    //针对不同的yuv格式获取不同的片段着色器
    void getShaderForYuv420p();

    void getShaderForNV21();

    void getShaderForNV12();

    /**
     * 加载片段着色器
     * @param source
     * @param type
     * @return
     */
    GLuint initFragShader(const char *source);
};


#endif //MANCHESTERUNITEDPLAYER_FILTER_H
