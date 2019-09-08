//
// Created by yanxi on 2019/9/1.
//

#include "IObserver.h"

void IObserver::AddOberver(IObserver *observer) {
    if (!observer) {
        return;
    }
    mutex.lock();
    obervers.push_back(observer);
    mutex.unlock();
}

void IObserver::notify(XData data) {
    mutex.lock();
    for (int i = 0; i < obervers.size(); ++i) {
        obervers[i]->Update(data);
    }
    mutex.unlock();
}