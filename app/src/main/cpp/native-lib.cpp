#include <jni.h>
#include <string>
#include "IDemux.h"
#include "FFDemux.h"
#include "XLog.h"
#include "IDecode.h"
#include "FFDecode.h"
#include "XEGL.h"
#include "XShader.h"
#include <android/native_window_jni.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_man_manchesterunitedplayer_MainActivity_startPlay(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    IDemux *iDemux = new FFDemux();
    iDemux->Open("/sdcard/v1080.mp4");

    IDecode *audioDecode = new FFDecode();
    audioDecode->Open(iDemux->GetAudioParameter());

    IDecode *videoDecode = new FFDecode();
    videoDecode->Open(iDemux->GetVideoParameter());

    iDemux->AddOberver(audioDecode);
    iDemux->AddOberver(videoDecode);

    iDemux->Start();
    audioDecode->Start();
    videoDecode->Start();

    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_man_manchesterunitedplayer_ManchesterPlayer_initView(JNIEnv *env, jobject thiz,
                                                              jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    XEGL::Get()->Init(win);
    XShader shader;
    shader.Init();
}