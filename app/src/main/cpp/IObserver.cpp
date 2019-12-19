//
// Created by yanxi on 2019/9/1.
//

#include "IObserver.h"
#include "XLog.h"

void IObserver::AddOberver(IObserver *observer) {
    if (!observer) {
        return;
    }
    //mutex.lock();
    const std::lock_guard<std::mutex> lock(mutex);
    obervers.push_back(observer);
   // mutex.unlock();
}

void IObserver::notify(XData data) {
   // mutex.lock();
    LOGLOCK("notify lock() start");
    const std::lock_guard<std::mutex> lock(mutex);
    LOGLOCK("notify lock() end");

    for (int i = 0; i < obervers.size(); ++i) {
        obervers[i]->Update(data);
    }
  //  mutex.unlock();
    LOGLOCK("notify unlock()3");

}