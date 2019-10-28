#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "FFPlayBuilder.h"

static IPlayer *player = nullptr;
//当当前动态库被load的时候会调用该函数
extern "C" JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    //FFDecode::InitHard(vm);
    FFPlayBuilder::InitHard(vm);
    player = FFPlayBuilder::Get()->BuildPlayer();
    player->Open("/sdcard/v1080.mp4");
    player->Start();
    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_man_manchesterunitedplayer_MainActivity_startPlay(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";



//    iDemux->Start();
//    audioDecode->Start();
//    videoDecode->Start();

    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_ManchesterPlayer_initView(JNIEnv *env, jobject thiz,
                                                              jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    //iVideoView->SetRender(win);
    if (player) {
        player->InitView(win);
    }


//    XEGL::Get()->Init(win);
//    XShader shader;
//    shader.Init();

}