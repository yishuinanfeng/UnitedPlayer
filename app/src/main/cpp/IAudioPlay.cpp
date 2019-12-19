//
// Created by yanxi on 2019/10/20.
//

#include <thread>
#include "IAudioPlay.h"
#include "XLog.h"

void IAudioPlay::Update(XData xData) {
    LOGD("IAudioPlay::Update data size:%d", xData.size);
    if (xData.size <= 0 || !xData.data) {
        return;
    }
    //如果framelist满了，则一直循环（类似自旋）
    while (!isExit) {
        //frameMutex.lock();
        LOGLOCK("IAudioPlay Update packsMutex.lock() start");
        const std::lock_guard<std::mutex> lock(frameMutex);
        LOGLOCK("IAudioPlay Update packsMutex.lock() end");
        if (framelist.size() >= maxFrame) {
            //  frameMutex.unlock();
            LOGLOCK("IAudioPlay framelist.size() >= maxFrame");
            LOGLOCK("IAudioPlay Update packsMutex.unlock()1");
            Sleep(1);
            //这里如果死暂停则跳出循环，否则会导致IDecode发生死锁！！！！！
            if (!IsPause()) {
                break;
            }
            continue;
        }
        framelist.push_back(xData);
        //      frameMutex.unlock();
        LOGLOCK("IAudioPlay Update packsMutex.unlock()2");
        break;
    }
}


XData IAudioPlay::GetData() {
    XData data;
    //todo
    isRunnig = true;
    while (!isExit) {

        if (IsPause()) {
            Sleep(2);
            LOGE("IAudioPlay::GetData Pause");
            continue;
        }

        LOGE("IAudioPlay::GetData resume");

        //   frameMutex.lock();
        const std::lock_guard<std::mutex> lock(frameMutex);
        if (!framelist.empty()) {
            data = framelist.front();
            framelist.pop_front();
            //   frameMutex.unlock();
            pst = data.pts;
            return data;
        }
        //    frameMutex.unlock();
        //停顿下，为了减轻cpu负荷
        Sleep(1);
    }
    //未获取到数据
    isRunnig = false;
    return data;
}

void IAudioPlay::Clear() {
    //  frameMutex.lock();
    LOGLOCK("IAudioPlay Clear packsMutex.lock() start");
    const std::lock_guard<std::mutex> lock(frameMutex);
    LOGLOCK("IAudioPlay Clear packsMutex.lock() end");
    while (!framelist.empty()) {
        framelist.front().Drop();
        framelist.pop_front();
    }

    LOGLOCK("IAudioPlay Clear packsMutex.unlock()");
    //   frameMutex.unlock();
}
