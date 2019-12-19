//
// Created by yanxi on 2019/10/20.
//
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "SLAudioPlay.h"
#include "XLog.h"

//static表示只在本cpp文件有效
static SLObjectItf objectItf = NULL;
static SLEngineItf eng = NULL;
static SLObjectItf mix = NULL;
//播放器对象
static SLObjectItf player = NULL;
//播放器接口
static SLPlayItf iPlayer = NULL;
static SLAndroidSimpleBufferQueueItf pcmQueue = NULL;

//static函数表示只在本cpp文件有效
static SLEngineItf CreateSl() {
    SLresult sLresult;
    SLEngineItf engineItf;
    sLresult = slCreateEngine(&objectItf, 0, 0, 0, 0, 0);
    if (sLresult != SL_RESULT_SUCCESS) {
        return NULL;
    }
    //阻塞式实例化引擎对象
    sLresult = (*objectItf)->Realize(objectItf, SL_BOOLEAN_FALSE);
    if (sLresult != SL_RESULT_SUCCESS) {
        return NULL;
    }
    sLresult = (*objectItf)->GetInterface(objectItf, SL_IID_ENGINE, &engineItf);
    if (sLresult != SL_RESULT_SUCCESS) {
        return NULL;
    }

    return engineItf;
}

/**
 * 获得缓存的解码重采样后的数据，将数据添加到OpenSl的播放队列中
 * @param bufq
 */
void SLAudioPlay::PlayCall(void *bufq) {
    if (!bufq) {
        LOGA("PlayCall fail");
        return;
    }
    LOGA("PlayCall");
    SLAndroidSimpleBufferQueueItf bufferQueueItf = static_cast<SLAndroidSimpleBufferQueueItf>(bufq);
    XData xData = GetData();
    if (xData.size <= 0) {
        LOGA("PlayCall GetData size is 0");
        return;
    }
    if (!buf) {
        LOGA("buf is NULL");
        return;
    }
    LOGA("PlayCall Enqueue");
    //保护buf？
  //  mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);

    memcpy(buf, xData.data, static_cast<size_t>(xData.size));
    if (pcmQueue && (*pcmQueue)) {
        (*bufferQueueItf)->Enqueue(bufferQueueItf, buf, static_cast<SLuint32>(xData.size));
    }
  //  mutex1.unlock();
    xData.Drop();
}

static void PcmCall(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *contex) {
    LOGA("PcmCall");
    SLAudioPlay *slAudioPlay = static_cast<SLAudioPlay *>(contex);
    if (!slAudioPlay) {
        LOGA("PcmCall contex is null");
        return;
    }

    slAudioPlay->PlayCall((void *) bufferQueueItf);
}

bool SLAudioPlay::StartPlay(XParameter out) {
    Close();
    //创建引擎
  //  mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);
    eng = CreateSl();
    if (eng) {
        LOGA("CreateSL success");
    } else {
        LOGA("CreateSL fail");
    //    mutex1.unlock();
        return false;
    }

    //2.创建混音器
    SLresult sLresult = 0;
    sLresult = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (sLresult != SL_RESULT_SUCCESS) {
        LOGA("CreateOutputMix fail");
      //  mutex1.unlock();
        return false;
    }

    sLresult = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (sLresult != SL_RESULT_SUCCESS) {
        LOGA("mix Realize fail");
   //     mutex1.unlock();
        return false;
    }

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink slDataSink = {&outputMix, 0};

    //3.配置音频信息
    //缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    //音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) out.channels,//声道数
            (SLuint32) out.sample_rate * 1000, //采样率
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//声道
            SL_BYTEORDER_LITTLEENDIAN  //小端模式
    };

    SLDataSource dataSource = {&queue, &pcm};

    //创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    //todo player初始值为NULL，怎么可以直接&player？
    sLresult = (*eng)->CreateAudioPlayer(eng, &player, &dataSource, &slDataSink, sizeof(ids) /
                                                                                 sizeof(SLInterfaceID),
                                         ids, req);
    if (sLresult == SL_RESULT_SUCCESS) {
        LOGA("CreateAudioPlayer success");
    } else {
        LOGA("CreateAudioPlayer fail");
    //    mutex1.unlock();
        return false;
    }

    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    sLresult = (*player)->GetInterface(player, SL_IID_PLAY, &iPlayer);
    if (sLresult == SL_RESULT_SUCCESS) {
        LOGA("GetInterface SL_IID_PLAY success");
    } else {
        LOGA("GetInterface SL_IID_PLAY fail");
    //    mutex1.unlock();
        return false;
    }
    //获得缓冲队列接口
    sLresult = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQueue);
    if (sLresult == SL_RESULT_SUCCESS) {
        LOGA("Create SL_IID_BUFFERQUEUE success");
    } else {
        LOGA("Create SL_IID_BUFFERQUEUE fail");
   //     mutex1.unlock();
        return false;
    }

    //设置回调函数，缓冲队列播放完调用PcmCall。
    //第三个参数为回调函数的参数
    //todo pcmQueue会传入回调函数第一个参数？
    (*pcmQueue)->RegisterCallback(pcmQueue, PcmCall, this);
    //设置为播放状态
    (*iPlayer)->SetPlayState(iPlayer, SL_PLAYSTATE_PLAYING);
    //加入空字符串启动队列回调
    (*pcmQueue)->Enqueue(pcmQueue, "", 1);
    LOGA("StartPlay success!");
    isExit = false;
  //  mutex1.unlock();
    return false;
}

SLAudioPlay::SLAudioPlay() {
    buf = new unsigned char[1024 * 1024];
}

SLAudioPlay::~SLAudioPlay() {
    delete buf;
}

void SLAudioPlay::Close() {
    Clear();
    //销毁播放器接口对象
  //  mutex1.lock();
    const std::lock_guard<std::mutex> lock(mutex1);
    //todo 为什么要判断*iPlayer？
    if (iPlayer && (*iPlayer)) {
        (*iPlayer)->SetPlayState(iPlayer, SL_PLAYSTATE_STOPPED);
    }
    //清理播放队列
    if (pcmQueue && (*pcmQueue)) {
        (*pcmQueue)->Clear(pcmQueue);
    }
    //销毁播放器对象
    if (player && (*player)) {
        (*player)->Destroy(player);
    }
    //销毁混音器
    if (mix && (*mix)) {
        (*mix)->Destroy(mix);
    }
    //销毁播放引擎对象（引擎接口也会跟着销毁）
    if (objectItf && (*objectItf)) {
        (*objectItf)->Destroy(objectItf);
    }

    //static表示只在本cpp文件有效
    objectItf = NULL;
    eng = NULL;
    mix = NULL;
//播放器对象
    player = NULL;
//播放器接口
    iPlayer = NULL;
    //todo pcmQueue要置为NULL，不然被清理了但是还会继续在音频回调函数中执行导致产生异常
    pcmQueue = NULL;
 //   mutex1.unlock();
}




