//
// Created by yanxi on 2019/8/31.
//

#ifndef MANCHESTERUNITEDPLAYER_XTHREAD_H
#define MANCHESTERUNITEDPLAYER_XTHREAD_H

//线程sleep毫秒
void Sleep(int mis);

class XThread {
public:
    //启动线程
    virtual void Start();

    //停止线程，通过标志位（不一定成功）
    virtual void Stop();

    //入口主函数
    virtual void Main() {}

protected:
    bool isExit = false;
    bool isRunnig = false;
private:
    void ThreadMain();
};


#endif //MANCHESTERUNITEDPLAYER_XTHREAD_H
