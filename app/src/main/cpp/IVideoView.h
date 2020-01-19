//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_IVIDEOVIEW_H
#define MANCHESTERUNITEDPLAYER_IVIDEOVIEW_H


#include "XData.h"
#include "IObserver.h"

class IVideoView :public IObserver{
public:
    virtual void SetRender(void *win) = 0;
    virtual void Render(XData xData) = 0;
    virtual void Update(XData xData);
    virtual void Close() = 0;
    virtual void setFilterType(int surfaceType) = 0;
    virtual void setScreenDimension(int width,int height) = 0;
};


#endif //MANCHESTERUNITEDPLAYER_IVIDEOVIEW_H
