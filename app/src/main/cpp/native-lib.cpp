#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "IPlayerProxy.h"

static IPlayer *player = nullptr;
//当当前动态库被load的时候会调用该函数
extern "C" JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    //FFDecode::InitHard(vm);
    IPlayerProxy::Get()->Init(vm);
//    IPlayerProxy::Get()->Open("/sdcard/v1080.mp4");
//    IPlayerProxy::Get()->Start();
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_ManchesterPlayer_initView(JNIEnv *env, jobject thiz,
                                                              jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    IPlayerProxy::Get()->InitView(win);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_OpenUrlActivity_open(JNIEnv *env, jobject thiz, jstring url) {
    // TODO: implement open()
    const char* openUrl = env->GetStringUTFChars(url,0);
    IPlayerProxy::Get()->Open(openUrl);
    IPlayerProxy::Get()->Start();
}