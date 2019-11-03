//
// Created by yanxi on 2019/9/15.
//

#include <android/native_window.h>
#include "XEGL.h"
#include "XLog.h"
#include <EGL/egl.h>
#include <mutex>

class CXEGL : public XEGL {
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;
    std::mutex mutex;

    virtual void Draw() {
        mutex.lock();
        if (display == EGL_NO_DISPLAY || eglSurface == EGL_NO_SURFACE) {
            return;
        }
        LOGE("CXEGL Draw");
        //将surface显示出来
        eglSwapBuffers(display, eglSurface);
        mutex.unlock();
    }

    virtual void Close() {
        mutex.lock();
        if (display == EGL_NO_DISPLAY) {
            mutex.unlock();
            return;
        }
        //display和surface解绑定
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(display, eglSurface);
        }
        if (eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(display, eglContext);
        }
        eglTerminate(display);
        display = EGL_NO_DISPLAY;
        eglSurface = EGL_NO_SURFACE;
        eglContext = EGL_NO_CONTEXT;
        mutex.unlock();
    }

    virtual bool Init(void *win) {
        Close();
        ANativeWindow *nwin = static_cast<ANativeWindow *>(win);
        mutex.lock();
        //初始化egl

        //1.获取egl display
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            LOGI("eglGetDisplay failed!");
            mutex.unlock();
            return false;
        }
        LOGI("eglGetDisplay success!");
        //2.初始化display，后两个参数为主次版本号
        if (EGL_TRUE != eglInitialize(display, 0, 0)) {
            LOGI("eglInitialize failed!");
            mutex.unlock();
            return false;
        }

        LOGI("eglInitialize success!");

        //3.1 surface配置，可以理解为窗口
        EGLConfig eglConfig = 0;
        EGLint configNum = 0;
        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };

        if (EGL_TRUE != eglChooseConfig(display, configSpec, &eglConfig, 1, &configNum)) {
            LOGD("eglChooseConfig failed");
            mutex.unlock();
            return false;
        }

        LOGD("eglCreateWindowSurface success");

        //3.2创建surface(egl和NativeWindow进行关联。最后一个参数为属性信息，0表示默认版本)
        eglSurface = eglCreateWindowSurface(display, eglConfig, nwin, NULL);
        if (eglSurface == EGL_NO_SURFACE) {
            LOGD("eglCreateWindowSurface failed");
            mutex.unlock();
            return false;
        }

        LOGD("eglCreateWindowSurface success");

        //4 创建关联上下文
        const EGLint ctxAttr[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
        };
        //EGL_NO_CONTEXT表示不需要多个设备共享上下文
        eglContext = eglCreateContext(display, eglConfig, EGL_NO_CONTEXT, ctxAttr);
        if (eglContext == EGL_NO_CONTEXT) {
            LOGD("eglCreateContext failed");
            mutex.unlock();
            return false;
        }

        LOGD("eglCreateContext success");
        //将egl和opengl关联
        //todo 两个surface一个读一个写。第二个一般用来离线渲染？
        if (EGL_TRUE != eglMakeCurrent(display, eglSurface, eglSurface, eglContext)) {
            LOGD("eglMakeCurrent failed");
            mutex.unlock();
            return false;
        }

        LOGD("eglMakeCurrent success");
        mutex.unlock();
        return true;
    }
};

/**
 * 单例创建对象
 * @return
 */
XEGL *XEGL::Get() {
    static CXEGL cxegl;
    return &cxegl;
}


