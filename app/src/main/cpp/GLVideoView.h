//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_GLVIDEOVIEW_H
#define MANCHESTERUNITEDPLAYER_GLVIDEOVIEW_H


#include "XData.h"
#include "IVideoView.h"

class XTexture;

class GLVideoView : public IVideoView {
public:
    virtual void SetRender(void *win);

    virtual void Render(XData xData);
    //todo 没有调用？？
    virtual void Close();

protected:
    void *view = 0;
    XTexture *xTexture = 0;
    std::mutex mutex1;
};


#endif //MANCHESTERUNITEDPLAYER_GLVIDEOVIEW_H
