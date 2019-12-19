//
// Created by yanxi on 2019/10/29.
//

#include "IPlayerProxy.h"
#include "FFPlayBuilder.h"

void IPlayerProxy::Init(void *vm) {
    //mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (vm) {
        FFPlayBuilder::InitHard(vm);
    }
    if (!player) {
        player = FFPlayBuilder::Get()->BuildPlayer();
    }
  //  mutexProxy.unlock();
}

bool IPlayerProxy::Open(const char *path) {
    bool result = false;
 //   mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        //todo 防止外部程序修改了isHardDecode无效
        player->isHardDecode = isHardDecode;
        result = player->Open(path);
    }
  //  mutexProxy.unlock();
    return result;
}

bool IPlayerProxy::Start() {
    bool result = false;
  //  mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        result = player->Start();
    }
 //   mutexProxy.unlock();
    return result;
}

bool IPlayerProxy::InitView(void *win) {
    bool result = false;
  //  mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        result = player->InitView(win);
    }
 //   mutexProxy.unlock();
    return result;
}

IPlayerProxy *IPlayerProxy::Get() {
    static IPlayerProxy iPlayerProxy;
    return &iPlayerProxy;
}

IPlayerProxy::IPlayerProxy() {

}

void IPlayerProxy::Close() {
  //  mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        player->Close();
    }
  //  mutexProxy.unlock();
}

double IPlayerProxy::GetPlayPose() {
    double pos = 0.0;
 //   mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player) {
        pos = player->GetPlayPose();
    }
  //  mutexProxy.unlock();
    return pos;
}

bool IPlayerProxy::Seek(double position) {
    bool re = false;
  //  mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player){
        re = player->Seek(position);
    }
 //   mutexProxy.unlock();
    return re;
}

void IPlayerProxy::SetPause(bool isP) {
 //   mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player){
        player->SetPause(isP);
    }
 //   mutexProxy.unlock();
}

bool IPlayerProxy::IsPause() {
    bool re = false;
  //  mutexProxy.lock();
    const std::lock_guard<std::mutex> lock(mutexProxy);
    if (player){
        re = player->IsPause();
    }
//    mutexProxy.unlock();
    return re;
}
