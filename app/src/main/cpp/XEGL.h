//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_XEGL_H
#define MANCHESTERUNITEDPLAYER_XEGL_H


class XEGL {
public:
    virtual bool Init(void* win) = 0;
    static XEGL *Get();
protected:
    XEGL(){}
};


#endif //MANCHESTERUNITEDPLAYER_XEGL_H
