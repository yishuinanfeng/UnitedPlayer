//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_TEXTUREHANDLER_H
#define MANCHESTERUNITEDPLAYER_TEXTUREHANDLER_H

enum XTextureType {
    XTEXTURE_YUV420P = 0,//每4个像素Y4 U1 V1  (测试中是软解码结果的格式)
    XTEXTURE_NV12 = 25, //每4个像素Y4 UV1   (测试中是硬解码解码结果的格式)
    XTEXTURE_NV21 = 26 //每4个像素Y4 VU1
};

class TextureHandler {
public:
    static TextureHandler *Create();

    virtual bool Init(void *win, XTextureType textureType, int i) = 0;

    virtual void Draw(unsigned char *data[], int videoWidth, int videoHeight, int filterType, int screenWidth, int screenHeight) = 0;

    virtual void Drop() = 0;
    //todo 这里使用virtual是因为父类指针delete的时候没有virtual是调用不到该析构函数的
    virtual ~TextureHandler(){};

    virtual void adjustVideoDimension(int videoWidth, int videoHeight, int screenWidth, int screenHeight) = 0;

protected:
    TextureHandler(){};
};


#endif //MANCHESTERUNITEDPLAYER_TEXTUREHANDLER_H
