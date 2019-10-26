//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_XTEXTURE_H
#define MANCHESTERUNITEDPLAYER_XTEXTURE_H

enum XTextureType{
    XTEXTURE_YUV420P = 0,//每4个像素Y4 U1 V1  (测试中是软解码结果的格式)
    XTEXTURE_NV12 = 25, //每4个像素Y4 UV1   (测试中是硬解码解码结果的格式)
    XTEXTURE_NV21 = 26 //每4个像素Y4 VU1
};

class XTexture {
public:
    static XTexture *Create();
    virtual bool Init(void *win,XTextureType textureType = XTEXTURE_YUV420P) = 0;
    virtual void Draw(unsigned char* data[],int width,int height) = 0;
};


#endif //MANCHESTERUNITEDPLAYER_XTEXTURE_H
