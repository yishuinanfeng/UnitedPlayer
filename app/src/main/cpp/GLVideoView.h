//
// Created by yanxi on 2019/9/15.
//

#ifndef MANCHESTERUNITEDPLAYER_GLVIDEOVIEW_H
#define MANCHESTERUNITEDPLAYER_GLVIDEOVIEW_H


#include "XData.h"
#include "IVideoView.h"

class TextureHandler;

class GLVideoView : public IVideoView {
public:
    virtual void SetRender(void *win);

    virtual void Render(XData xData);

    virtual void Close();

    virtual void setFilterType(int filterType);

    virtual void setScreenDimension(int width,int height);

    GLVideoView();


protected:
    void *view = 0;
    TextureHandler *xTexture = 0;
    std::mutex mutex1;
    int filterType = -1;
    int screenWidth = 0;
    int screenHeight = 0;
};


#endif //MANCHESTERUNITEDPLAYER_GLVIDEOVIEW_H
