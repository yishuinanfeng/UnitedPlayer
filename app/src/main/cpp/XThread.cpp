//
// Created by yanxi on 2019/8/31.
//

#include "XThread.h"
#include "XLog.h"
#include <thread>

using namespace std;

/**
 * 延时，本质为释放cpu资源，让cpu休息
 * @param mis
 */
void Sleep(int mis) {
    chrono::microseconds du(mis);
    this_thread::sleep_for(du);
}

//启动线程
bool XThread::Start() {
    //创建线程
    isExit = false;
    isPause = false;
    thread th(&XThread::ThreadMain, this);
    //当前线程成为守护线程
    th.detach();
    return true;
}

//停止线程，通过标志位（不一定成功）
void XThread::Stop() {
    isExit = true;
    //200毫秒的退出超时时间
    for (int i = 0; i < 200; ++i) {
        if (!isRunnig){
            return;
        }
        Sleep(1);
    }
    LOGD("stop线程超时！");
}

//入口主函数
void XThread::ThreadMain() {
    isRunnig = true;
    LOGI("线程函数进入");
    Main();
    LOGI("线程函数退出");
    isRunnig = false;
}

void XThread::SetPause(bool pause) {
    //todo 这里c++是否存在多线程的可见性问题呢？
    LOGXThread("SetPause pause:%d,threadName:%s",pause,threadName);
    isPause = pause;
}

bool XThread::IsPause() {
    return isPause;
}
