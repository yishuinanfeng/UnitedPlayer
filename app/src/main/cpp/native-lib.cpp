#include <jni.h>
#include <string>
#include "IDemux.h"
#include "FFDemux.h"
#include "XLog.h"
#include "IDecode.h"
#include "FFDecode.h"
#include "XEGL.h"
#include "XShader.h"
#include "IVideoView.h"
#include "GLVideoView.h"
#include "IResample.h"
#include "FFResample.h"
#include <android/native_window_jni.h>

IVideoView *iVideoView = NULL;

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

    //解复用一帧之后，通知解码器
    iDemux->AddOberver(audioDecode);
    iDemux->AddOberver(videoDecode);

    iVideoView = new GLVideoView();
    //解码一帧之后，通知显示模块
    videoDecode->AddOberver(iVideoView);

    IResample *resample = new FFResample();
    resample->Open(iDemux->GetAudioParameter());
    audioDecode->AddOberver(resample);

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
    iVideoView->SetRender(win);



//    XEGL::Get()->Init(win);
//    XShader shader;
//    shader.Init();

}