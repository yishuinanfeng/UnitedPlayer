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
        xTexture-> Init(view, static_cast<XTextureType>(xData.format));
    }
    xTexture->Draw(xData.datas, xData.width, xData.height, xData.pts);
    LOG_VIDEO_DIMENSION("xData.width:%d, xData.height:%d",xData.width, xData.height);
}

void GLVideoView::SetRender(void *win) {
    view = win;
    LOGE("SetRender");


//    if (!xTexture) {
//        xTexture = XTexture::Create();
//        xTexture->Init(view);
//    }
}

void GLVideoView::Close() {
  //  mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);
    if (xTexture) {
        xTexture->Drop();
        xTexture = nullptr;
    }
 //   mutex1.unlock();
}
