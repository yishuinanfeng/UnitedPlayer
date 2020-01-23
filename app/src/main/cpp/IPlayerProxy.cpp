//
// Created by yanxi on 2019/10/29.
//

#include "IPlayerProxy.h"
#include "FFPlayBuilder.h"

void IPlayerProxy::Init(void *vm) {

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (vm) {
        FFPlayBuilder::InitHard(vm);
    }
    if (!player) {
        player = FFPlayBuilder::Get()->BuildPlayer();
    }

}

bool IPlayerProxy::Open(const char *path) {
    bool result = false;

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        //todo 防止外部程序修改了isHardDecode无效
        player->isHardDecode = isHardDecode;
        result = player->Open(path);
    }

    return result;
}

bool IPlayerProxy::Start() {
    bool result = false;

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        result = player->Start();
    }

    return result;
}

bool IPlayerProxy::InitView(void *win, int filterType, int screenWidth, int screenHeight) {
    bool result = false;

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        result = player->InitView(win, filterType, screenWidth, screenHeight);
    }

    return result;
}

IPlayerProxy *IPlayerProxy::Get() {
    static IPlayerProxy iPlayerProxy;
    return &iPlayerProxy;
}

IPlayerProxy::IPlayerProxy() {

}

void IPlayerProxy::Close() {

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        player->Close();
    }

}

double IPlayerProxy::GetPlayPose() {
    double pos = 0.0;

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        pos = player->GetPlayPose();
    }

    return pos;
}

bool IPlayerProxy::Seek(double position) {
    bool re = false;

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        re = player->Seek(position);
    }

    return re;
}

void IPlayerProxy::SetPause(bool isP) {

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        player->SetPause(isP);
    }

}

bool IPlayerProxy::IsPause() {
    bool re = false;

    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        re = player->IsPause();
    }

    return re;
}

void IPlayerProxy::setFilterType(int filterType) {
    player->setFilterType(filterType);
}


