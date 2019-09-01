//
// Created by yanxi on 2019/8/31.
//

#include "XThread.h"
#include "XLog.h"
#include <thread>

using namespace std;

//启动线程
void XThread::Start() {
    //创建线程
    thread th(&XThread::ThreadMain, this);
    th.detach();
}

//停止线程，通过标志位（不一定成功）
void XThread::Stop() {

}

//入口主函数
void XThread::ThreadMain() {
    LOGI("线程函数进入");
    Main();
    LOGI("线程函数退出");
}
