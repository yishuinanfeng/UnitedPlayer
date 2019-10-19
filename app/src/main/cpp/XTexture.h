//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_XTEXTURE_H
#define MANCHESTERUNITEDPLAYER_XTEXTURE_H


class XTexture {
public:
    static XTexture *Create();
    virtual bool Init(void *win) = 0;
    virtual void Draw(unsigned char* data[],int width,int height) = 0;
};


#endif //MANCHESTERUNITEDPLAYER_XTEXTURE_H
