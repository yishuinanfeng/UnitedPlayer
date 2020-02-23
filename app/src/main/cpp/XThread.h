//
// Created by yanxi on 2019/8/31.
//

#ifndef MANCHESTERUNITEDPLAYER_XTHREAD_H
#define MANCHESTERUNITEDPLAYER_XTHREAD_H

#include <string>

//线程sleep毫秒
void Sleep(int mis);

class XThread {
public:
    //启动线程
    virtual bool Start();

    //停止线程，通过标志位（不一定成功）
    virtual void Stop();

    virtual void SetPause(bool isPause);

    //入口主函数
    virtual void Main() {}

    virtual bool IsPause();


protected:
    char *threadName = nullptr;
    bool isExit = false;
    bool isRunnig = false;
    bool isPause = true;
    bool isPausing = false;
private:
    void ThreadMain();
};


#endif //MANCHESTERUNITEDPLAYER_XTHREAD_H
