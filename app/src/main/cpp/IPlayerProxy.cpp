//
// Created by yanxi on 2019/10/29.
//

#include "IPlayerProxy.h"
#include "FFPlayBuilder.h"

void IPlayerProxy::Init(void *vm) {
    mutex.lock();
    if (vm) {
        FFPlayBuilder::InitHard(vm);
    }
    if (!player) {
        player = FFPlayBuilder::Get()->BuildPlayer();
    }
    mutex.unlock();
}

bool IPlayerProxy::Open(const char *path) {
    bool result = false;
    mutex.lock();
    if (player) {
        result = player->Open(path);
    }
    mutex.unlock();
    return result;
}

bool IPlayerProxy::Start() {
    bool result = false;
    mutex.lock();
    if (player) {
        result = player->Start();
    }
    mutex.unlock();
    return result;
}

bool IPlayerProxy::InitView(void *win) {
    bool result = false;
    mutex.lock();
    if (player) {
        result = player->InitView(win);
    }
    mutex.unlock();
    return result;
}

IPlayerProxy *IPlayerProxy::Get() {
    static IPlayerProxy iPlayerProxy;
    return &iPlayerProxy;
}

IPlayerProxy::IPlayerProxy() {

}
