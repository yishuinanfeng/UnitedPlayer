//
// Created by yanxi on 2019/9/15.
//

#include "IVideoView.h"
#include "XLog.h"

void IVideoView::Update(XData xData) {
    LOGE("IVideoView Update");
    this->Render(xData);
}
