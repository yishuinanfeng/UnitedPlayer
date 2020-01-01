#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#include "RecordBuffer.h"
#include<iostream>


//todo 这里相当于创建了一个SLObjectItf_（看SLObjectItf定义处）的二级指针，指向一个NULL？
SLObjectItf slObjectEngine = NULL;
SLEngineItf slEngineItf = NULL;
SLObjectItf recordObj = NULL;
SLRecordItf recordItf = NULL;

SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf = NULL;

RecordBuffer *recordBuffer;

FILE *pcmFile = NULL;

bool isFinish = false;
JNIEnv *envGlobal = NULL;
JavaVM *javaVM;

jobject mAGlobalObject = NULL;;
jclass mainActivityClass = NULL;;

void recordDataCallBack(short *pInt);

const int static BUFFER_SIZE = 1024;

#define LOGD(...) __android_log_print(ANDROID_LOG_WARN,"OpenSlDemo",__VA_ARGS__)

void recordCallBack(SLAndroidSimpleBufferQueueItf caller, void *pContext) {
    //写入4096次，每次写一个字节
    //此时recordBuffer->getNowBuffer()中已经存储了录制到的数据了
    LOGD("recordCallBack");

    recordDataCallBack(recordBuffer->getNowBuffer());
    //todo 传数据给Java层
    //fwrite(recordBuffer->getNowBuffer(), 1, BUFFER_SIZE, pcmFile);
    if (isFinish) {
        (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_STOPPED);
        (*recordObj)->Destroy(recordObj);
        recordObj = NULL;
        recordItf = NULL;
        (*slObjectEngine)->Destroy(slObjectEngine);
        slObjectEngine = NULL;
        slEngineItf = NULL;
        delete (recordBuffer);
    } else {
        //一次录制4096数据到recordBuffer->getRecordBuffer()中
        (*slAndroidSimpleBufferQueueItf)->Enqueue(slAndroidSimpleBufferQueueItf,
                                                  recordBuffer->getRecordBuffer(), BUFFER_SIZE);
    }

}

void recordDataCallBack(short *pInt) {
    JNIEnv *env;
    bool needDetach = false;
    int envStatus = javaVM->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (envStatus == JNI_EDETACHED) {
        if (javaVM->AttachCurrentThread(&env, NULL) != 0) {
            return;
        }
        needDetach = JNI_TRUE;
    }

    jclass mainClass = env->GetObjectClass(mAGlobalObject);
    if (mainClass == NULL) {
        LOGD("mainClass is null");
        javaVM->DetachCurrentThread();
        return;
    }

    LOGD("GetMethodID");
    jmethodID pcmDataCallBack = env->GetMethodID(mainClass, "onPcmDataInput",
                                                 "([B)V");
    LOGD("ExceptionCheck");
    if (env->ExceptionCheck()) {
        LOGD("GetMethodID exception!");
    }

    jbyteArray pcmArray = env->NewByteArray(BUFFER_SIZE);
    env->SetByteArrayRegion(pcmArray, 0, BUFFER_SIZE, reinterpret_cast<const jbyte *>(pInt));

    LOGD("CallVoidMethod");
    env->CallVoidMethod(mAGlobalObject, pcmDataCallBack, pcmArray);
    LOGD("CallVoidMethod onPcmDataInput!");

    if (needDetach) {
        javaVM->DetachCurrentThread();
    }

    env = NULL;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_haha_record_VideoActivity_startRecord(JNIEnv *env, jobject thiz) {

    env->GetJavaVM(&javaVM);
    mAGlobalObject = env->NewGlobalRef(thiz);
    LOGD("new RecordBuffer");
    recordBuffer = new RecordBuffer(BUFFER_SIZE);
    LOGD("slCreateEngine");
    slCreateEngine(&slObjectEngine, 0, NULL, 0, NULL, NULL);
    LOGD("Realize");
    (*slObjectEngine)->Realize(slObjectEngine, SL_BOOLEAN_FALSE);
    LOGD("GetInterface");
    (*slObjectEngine)->GetInterface(slObjectEngine, SL_IID_ENGINE, &slEngineItf);
    //输入设备属性配置
    LOGD("locatorIoDevice");
    SLDataLocator_IODevice locatorIoDevice = {SL_DATALOCATOR_IODEVICE,
                                              SL_IODEVICE_AUDIOINPUT,
                                              SL_DEFAULTDEVICEID_AUDIOINPUT,
                                              NULL};
    SLDataSource audioSrc = {&locatorIoDevice, NULL};
    LOGD("dataLocatorAndroidSimpleBufferQueue");
    SLDataLocator_AndroidSimpleBufferQueue dataLocatorAndroidSimpleBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            2
    };
    LOGD("slDataFormatPcm");
    //音频格式，声道数，采样率，位深，位深，声道布局，内存对齐方式
    SLDataFormat_PCM slDataFormatPcm = {
            SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1, SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSink slDataSink = {
            &dataLocatorAndroidSimpleBufferQueue,
            &slDataFormatPcm
    };

    //加的功能点（这里只加了队列，还有环绕、均衡器等功能可以加）
    const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    LOGD("CreateAudioRecorder");
    (*slEngineItf)->CreateAudioRecorder(slEngineItf, &recordObj, &audioSrc, &slDataSink, 1, id,
                                        req);
    LOGD("Realize recordObj");
    (*recordObj)->Realize(recordObj, SL_BOOLEAN_FALSE);
    LOGD("GetInterface(recordObj");
    (*recordObj)->GetInterface(recordObj, SL_IID_RECORD, &recordItf);
    LOGD("GetInterface(recordObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE");
    (*recordObj)->GetInterface(recordObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                               &slAndroidSimpleBufferQueueItf);
    (*slAndroidSimpleBufferQueueItf)->Enqueue(slAndroidSimpleBufferQueueItf,
                                              recordBuffer->getRecordBuffer(), BUFFER_SIZE);
    (*slAndroidSimpleBufferQueueItf)->RegisterCallback(slAndroidSimpleBufferQueueItf,
                                                       recordCallBack, NULL);
    (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_RECORDING);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_haha_record_VideoActivity_stopRecord(JNIEnv *env, jobject thiz) {
    isFinish = true;
}
