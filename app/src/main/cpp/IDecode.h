//
// Created by yanxi on 2019/9/1.
//

#ifndef MANCHESTERUNITEDPLAYER_IDECODE_H
#define MANCHESTERUNITEDPLAYER_IDECODE_H


#include "XParameter.h"
#include "IObserver.h"
#include <list>

/**
 * 负责解码的模块，具体的解码逻辑在子类
 */
class IDecode: public IObserver {
public:
    //打开解码器.isHard:视频是否需要硬解码
    virtual bool Open(XParameter xParameter, bool isHard = false) = 0;
    //Future模式
    virtual bool SendPacket(XData pkt) = 0;
    //从线程中获取解码结果
    virtual XData RecvFrame() = 0;
    //
    virtual void Update(XData pkt);
    //当前解码器是音频还是视频
    bool isAudio = false;
    //缓冲队列最大数量
    int packMaxCount = 100;

protected:
    virtual void Main();
    //存储缓冲帧
    std::list<XData> packList;
    //读取队列的互斥变量
    std::mutex packsMutex;
};


#endif //MANCHESTERUNITEDPLAYER_IDECODE_H
