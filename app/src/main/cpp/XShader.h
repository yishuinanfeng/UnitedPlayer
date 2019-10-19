//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_XSHADER_H
#define MANCHESTERUNITEDPLAYER_XSHADER_H


class XShader {
public:
    virtual bool Init();

    void GetTexture(unsigned int index,int width,int height, unsigned char *buf);
    //避免头文件引入了OpenGL依赖
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    //100层的纹理id数组
    unsigned int text[100] = {0};

    void Draw();
};


#endif //MANCHESTERUNITEDPLAYER_XSHADER_H
