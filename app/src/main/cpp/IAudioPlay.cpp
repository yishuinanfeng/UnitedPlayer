//
// Created by yanxi on 2019/10/20.
//

#include "IAudioPlay.h"
#include "XLog.h"

void IAudioPlay::Update(XData xData) {
    LOGD("IAudioPlay::Update data size:%d", xData.size);
    if (xData.size <= 0 || !xData.data) {
        return;
    }
    //如果framelist满了，则一直循环（类似自旋）
    while (!isExit) {
        frameMutex.lock();
        if (framelist.size() >= maxFrame) {
            frameMutex.unlock();
            Sleep(1);
            continue;
        }
        framelist.push_back(xData);
        frameMutex.unlock();
        break;
    }
}


XData IAudioPlay::GetData() {
    XData data;
    //todo
    while (!isExit) {
        frameMutex.lock();
        if (!framelist.empty()) {
            data = framelist.front();
            framelist.pop_front();
            frameMutex.unlock();
            pst = data.pts;
            return data;
        }
        frameMutex.unlock();
        //停顿下，为了减轻cpu负荷
        Sleep(1);
    }
    //未获取到数据
    return data;
}
