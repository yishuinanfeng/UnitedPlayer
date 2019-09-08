#include <jni.h>
#include <string>
#include "IDemux.h"
#include "FFDemux.h"
#include "XLog.h"
#include "IDecode.h"
#include "FFDecode.h"

class TestObserver:public IObserver{
public:
    void Update(XData data){
      //  LOGI("update date size is %d:",data.size);
    }
};

extern "C" JNIEXPORT jstring JNICALL
Java_com_man_manchesterunitedplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    IDemux *iDemux = new FFDemux();
    iDemux->Open("/sdcard/v1080.mp4");

    TestObserver *testObserver = new TestObserver();
    iDemux->AddOberver(testObserver);

    IDecode *decode = new FFDecode();
    decode->Open(iDemux->GetParameter());

    iDemux->Start();
    Sleep(3000);
    iDemux->Stop();

    return env->NewStringUTF(hello.c_str());
}
