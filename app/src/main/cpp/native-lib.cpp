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
                                                              jobject surface, jint filterType,
                                                              jint screenWidth, jint screenHeight) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    IPlayerProxy::Get()->InitView(win, filterType, screenWidth, screenHeight);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_ManchesterPlayer_setFilter(JNIEnv *env, jobject thiz, jint filterType
) {
    IPlayerProxy::Get()->setFilterType(filterType);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_ManchesterPlayer_open(JNIEnv *env, jobject thiz, jstring url) {
    // TODO: implement open()
    const char *openUrl = env->GetStringUTFChars(url, 0);
    IPlayerProxy::Get()->Open(openUrl);
    IPlayerProxy::Get()->Start();
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_man_manchesterunitedplayer_PlayActivity_getPlayPos(JNIEnv *env, jobject thiz) {
    // TODO: double可以直接返回？
    return IPlayerProxy::Get()->GetPlayPose();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_PlayActivity_seek(JNIEnv *env, jobject thiz, jdouble position) {
    // TODO: implement seek()
    IPlayerProxy::Get()->Seek(position);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_ManchesterPlayer_pausePlay(JNIEnv *env, jobject thiz) {
    // TODO: implement pausePlay()
    IPlayerProxy::Get()->SetPause(!IPlayerProxy::Get()->IsPause());
}extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_ManchesterPlayer_closePlay(JNIEnv *env, jobject thiz) {
    IPlayerProxy::Get()->Close();
}