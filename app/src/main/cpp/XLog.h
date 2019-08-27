//
// Created by yanxi on 2019/8/25.
//

#ifndef MANCHESTERUNITEDPLAYER_XLOG_H
#define MANCHESTERUNITEDPLAYER_XLOG_H


/**
 * 日志打印器接口类
 */
class XLog {

};
#ifdef ANDROID
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"yuvOpenGlDemo",__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"yuvOpenGlDemo",__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"yuvOpenGlDemo",__VA_ARGS__)
#else

#endif


#endif //MANCHESTERUNITEDPLAYER_XLOG_H
