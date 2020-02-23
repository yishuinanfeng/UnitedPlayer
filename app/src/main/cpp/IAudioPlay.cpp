//
// Created by yanxi on 2019/10/20.
//

#include <thread>
#include "IAudioPlay.h"
#include "XLog.h"

void IAudioPlay::Update(XData xData) {
    LOGAudioPlayer("IAudioPlay::Update data size:%d", xData.size);
    if (xData.size <= 0 || !xData.data) {
        return;
    }
    //如果framelist满了，则一直循环（类似自旋）
    while (!isExit) {
        const std::lock_guard<std::mutex> lock(frameMutex);

        if (framelist.size() >= maxFrame) {
            Sleep(1);
            //这里如果死暂停则跳出循环，否则会导致IDecode发生死锁！！！！！
            if (IsPause()) {
                break;
            }
            continue;
        }
        LOGAudioPlayer("framelist.push_back");
        framelist.push_back(xData);
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
            LOGAudioPlayer("IAudioPlay::GetData Pause");
            continue;
        }

     //   LOGAudioPlayer("IAudioPlay::GetData");

        const std::lock_guard<std::mutex> lock(frameMutex);
        LOGAudioPlayer("framelist.size:%d:",framelist.size());
        if (!framelist.empty()) {
            data = framelist.front();
            framelist.pop_front();
            pst = data.pts;
            LOGAudioPlayer("pst = data.pts，pst：%d",pst);
            return data;
        }
        //停顿下，为了减轻cpu负荷
        Sleep(1);
    }
    //未获取到数据
    isRunnig = false;
    return data;
}

void IAudioPlay::Clear() {
    const std::lock_guard<std::mutex> lock(frameMutex);

    while (!framelist.empty()) {
        framelist.front().Drop();
        framelist.pop_front();
    }

}
