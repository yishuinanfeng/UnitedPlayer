//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_XSHADER_H
#define MANCHESTERUNITEDPLAYER_XSHADER_H


class XShader {
public:
    virtual bool Init();
    //避免头文件引入了OpenGL依赖
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
};


#endif //MANCHESTERUNITEDPLAYER_XSHADER_H
