//
// Created by yanxi on 2019/9/1.
//

#include "IDecode.h"
#include "XLog.h"

//由主体notify发送的数据
//生产者
void IDecode::Update(XData pkt) {
    //媒体类型不属于当前的解码器
    if (pkt.isAudio != isAudio) {
        return;
    }
    LOGE("Update isAudio %d",isAudio);
    //如果队列满了，则不能添加元素，循环等待队列不满的时候再添加。阻塞队列的功能。
    while (!isExit) {
        packsMutex.lock();
        if (packList.size() < packMaxCount) {
            packList.push_back(pkt);
            packsMutex.unlock();
            break;
        }
        packsMutex.unlock();
        //延时是为了防止不断循环将cpu耗尽
        Sleep(1);
    }
}

/**
 * 从音视频帧缓冲队列获取帧，发送到解码线程并获取到解码返回的帧，再讲该帧通知给观察者
 */
void IDecode::Main() {
    while (!isExit) {
        packsMutex.lock();
        if (packList.empty()) {
            packsMutex.unlock();
            continue;
        }
        //消费者
        XData pack = packList.front();
        packList.pop_front();

        LOGE("Main pack size is %d",pack.size);

        if (this->SendPacket(pack)) {
            //发送一个Packet可能会Receive到多个Frame(一般是音频，视频一般一个Packet对应一个Frame)
            //所以要循环读取，将一个Packet对应的Frame读取完？
            while (!isExit) {
                XData frame = RecvFrame();
                if (!frame.data) {
                    break;
                }
                LOGE("RecvFrame size is %d",frame.size);

                notify(frame);
            }
        }
        //防止内存泄漏
        pack.Drop();
        packsMutex.unlock();
    }
}

