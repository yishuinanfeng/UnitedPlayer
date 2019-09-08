//
// Created by yanxi on 2019/9/1.
//

#ifndef MANCHESTERUNITEDPLAYER_FFDECODE_H
#define MANCHESTERUNITEDPLAYER_FFDECODE_H


#include "XParameter.h"
#include "IDecode.h"

struct AVCodecContext;

class FFDecode :public IDecode{
public:
    //打开解码器
    virtual bool Open(XParameter xParameter);

protected:
    AVCodecContext *codecContext = 0;
};


#endif //MANCHESTERUNITEDPLAYER_FFDECODE_H
