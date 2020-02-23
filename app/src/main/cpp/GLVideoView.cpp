//
// Created by yanxi on 2019/9/15.
//

#include "GLVideoView.h"
#include "TextureHandler.h"
#include "XLog.h"

void GLVideoView::Render(XData xData) {
    LOGE("Render start");
    if (!view) {
        return;
    }
    if (!xTexture) {
        xTexture = TextureHandler::Create();
        xTexture->Init(view, static_cast<XTextureType>(xData.format), filterType);
    }
    xTexture->Draw(xData.datas, xData.width, xData.height, xData.pts, screenWidth, screenHeight);
    LOG_VIDEO_DIMENSION("xData.width:%d, xData.height:%d", xData.width, xData.height);
}

void GLVideoView::SetRender(void *win) {
    view = win;
    LOGE("SetRender");
}

void GLVideoView::Close() {
    const std::lock_guard<std::mutex> lock(mutex1);
    if (xTexture) {
        xTexture->Drop();
        xTexture = nullptr;
    }
}

void GLVideoView::setFilterType(int filterType) {
    this->filterType = filterType;
}

void GLVideoView::setScreenDimension(int width, int height) {
    this->screenWidth = width;
    this->screenHeight = height;
}

GLVideoView::GLVideoView() {
    threadName = "GLVideo";
}
