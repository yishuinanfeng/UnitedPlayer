//
// Created by yanxi on 2019/9/1.
//

#ifndef MANCHESTERUNITEDPLAYER_IOBSERVER_H
#define MANCHESTERUNITEDPLAYER_IOBSERVER_H

#include "XData.h"
#include "XThread.h"
#include <vector>
#include <mutex>

/**
 * 观察者
 */
class IObserver :public XThread{
public:
    virtual void Update(XData data){}

    void AddOberver(IObserver *observer);

    void notify(XData data);

private:
    std::vector<IObserver *> obervers;
    std::mutex mutex;
};


#endif //MANCHESTERUNITEDPLAYER_IOBSERVER_H
