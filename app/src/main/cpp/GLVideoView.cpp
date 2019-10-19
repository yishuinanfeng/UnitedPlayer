//
// Created by yanxi on 2019/9/15.
//

#include "GLVideoView.h"
#include "XTexture.h"
#include "XLog.h"

void GLVideoView::Render(XData xData) {
    LOGE("Render start");
    if (!view) {
        return;
    }
    if (!xTexture) {
        xTexture = XTexture::Create();
        xTexture->Init(view);
    }
    xTexture->Draw(xData.datas, xData.width, xData.height);
    LOGE("Render end");
}

void GLVideoView::SetRender(void *win) {
    view = win;
    LOGE("SetRender");


//    if (!xTexture) {
//        xTexture = XTexture::Create();
//        xTexture->Init(view);
//    }
}
