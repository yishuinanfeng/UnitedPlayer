#include <jni.h>
#include <string>
#include "IDemux.h"
#include "FFDemux.h"
#include "XLog.h"
#include "IDecode.h"
#include "FFDecode.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_man_manchesterunitedplayer_MainActivity_stringFromJNI(
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


//    Sleep(3000);
//    iDemux->Stop();

    return env->NewStringUTF(hello.c_str());
}
