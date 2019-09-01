//
// Created by yanxi on 2019/8/31.
//

#include "XThread.h"
#include "XLog.h"
#include <thread>

using namespace std;

void Sleep(int mis) {
    chrono::microseconds du(mis);
    this_thread::sleep_for(du);
}

//启动线程
void XThread::Start() {
    //创建线程
    isExit = false;
    thread th(&XThread::ThreadMain, this);
    th.detach();
}

//停止线程，通过标志位（不一定成功）
void XThread::Stop() {
    isExit = true;

}

//入口主函数
void XThread::ThreadMain() {
    isRunnig = true;
    LOGI("线程函数进入");
    Main();
    LOGI("线程函数退出");
    isRunnig = false;
}
