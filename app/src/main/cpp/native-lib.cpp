#include <jni.h>
#include <string>
#include "IDemux.h"
#include "FFDemux.h"
#include "XLog.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_man_manchesterunitedplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    IDemux *iDemux = new FFDemux();
    iDemux->Open("/sdcard/v1080.mp4");

    iDemux->Start();

    return env->NewStringUTF(hello.c_str());
}
