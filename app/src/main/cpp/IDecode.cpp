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
    LOGE("Update isAudio %d", isAudio);
    //如果队列满了，则不能添加元素，循环等待队列不满的时候再添加。阻塞队列的功能。
    while (!isExit) {
        LOGLOCK("Update packsMutex.lock() start");
        //    packsMutex.lock();
        const std::lock_guard<std::mutex> lock(packsMutex);
        LOGLOCK("Update packsMutex.lock() end");

        if (packList.size() < packMaxCount) {
            packList.push_back(pkt);
            //    packsMutex.unlock();
            LOGLOCK("Update packsMutex.unlock()");
            break;
        }
        //  packsMutex.unlock();
        LOGLOCK("Update packsMutex.unlock()");
        //延时是为了防止不断循环将cpu耗尽
        Sleep(1);
    }
}

/**
 * 从音视频帧缓冲队列获取帧，发送到解码线程并获取到解码返回的帧，再讲该帧通知给观察者
 */
void IDecode::Main() {
    while (!isExit) {
        if (IsPause()) {
            LOGE("IDecode::Main Pause");
            //    LOGLOCK("IsPause()");
            Sleep(2);
            continue;
        }


        //  packsMutex.lock();
        //todo 这个加锁语句和clear中的加锁语句导致死锁
        {
            LOGLOCK("Main packsMutex.lock() start");
            const std::lock_guard<std::mutex> lock(packsMutex);
            LOGLOCK("Main packsMutex.lock() end");

            //音视频同步
            if (!isAudio && synPts > 0) {
                //如果正在播放的音频帧的pts小于的刚解码出来的视频帧pts，则等待直到播放的音频帧不小于再解码视频帧
                //synPts为当前播放的音频帧pts
                if (synPts < pts) {
                    //  packsMutex.unlock();
                    LOGLOCK("synPts < pts");
                    LOGLOCK("Main packsMutex.unlock()1");
                    Sleep(1);
                    continue;
                }
            }

            if (packList.empty()) {
                //   packsMutex.unlock();
                LOGLOCK("packList.empty()");
                LOGLOCK("Main packsMutex.unlock()2");
                continue;
            }

            //消费者
            XData pack = packList.front();
            LOGLOCK("packList.front()");
            packList.pop_front();

            LOGE("Main pack size is %d", pack.size);

            LOGLOCK("SendPacket(pack) start");
            if (this->SendPacket(pack)) {
                LOGLOCK("SendPacket(pack) end");
                //发送一个Packet可能会Receive到多个Frame(一般是音频，视频一般一个Packet对应一个Frame)
                //所以要循环读取，将一个Packet对应的Frame读取完？
                while (!isExit) {
                    LOGLOCK("RecvFrame start");
                    XData frame = RecvFrame();
                    LOGLOCK("RecvFrame end");
                    if (!frame.data) {
                        break;
                    }
                    LOGE("RecvFrame size is %d", frame.size);

                    //每次解码后将当前帧的pts保存

                    pts = frame.pts;

                    notify(frame);

                    LOGLOCK("notify(frame)");
                }


            }

            //防止内存泄漏
            pack.Drop();
            LOGLOCK("pack.Drop()");
        }

        //  packsMutex.unlock();
        LOGLOCK("Main packsMutex.unlock()3");

    }
}

void IDecode::Clear() {
    LOGLOCK("Clear packsMutex.lock() start");
    //  packsMutex.try_lock();
    //todo 这个加锁语句和Main中的加锁语句导致死锁
    const std::lock_guard<std::mutex> lock(packsMutex);
    LOGLOCK("Clear packsMutex.lock() end");

    // bool a = packsMutex.try_lock();

    //  LOGLOCK("Clear packsMutex.lock() success a =%d",a);

    while (!packList.empty()) {
        packList.front().Drop();
        packList.pop_front();
    }
    pts = 0;
    synPts = 0;

    //  packsMutex.unlock();
    LOGLOCK("Clear packsMutex.unlock()");
}

