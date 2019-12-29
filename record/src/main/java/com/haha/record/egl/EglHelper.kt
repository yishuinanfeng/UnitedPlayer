package com.haha.record.egl

import android.opengl.EGL14

import android.view.Surface
import javax.microedition.khronos.egl.*

class EglHelper {

    private var mEgl: EGL10? = null
    private var mEglDisplay: EGLDisplay? = null
    private var mEglContext: EGLContext? = null
    private var mEglSurface: EGLSurface? = null

    fun initEgl(surface: Surface, eglContext: EGLContext?) {
        //1.得到EGL实例
        mEgl = EGLContext.getEGL() as EGL10

        //2.得到Display实例，是屏幕设备的抽象
        mEglDisplay = mEgl?.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY)
        if (mEglDisplay === EGL10.EGL_NO_DISPLAY) {
            throw RuntimeException("eglGetDisplay failed")
        }

        //3.初始化EGL对象
        val version = IntArray(2)
        if (!mEgl!!.eglInitialize(mEglDisplay, version)) {
            throw RuntimeException("eglInitialize failed")
        }

        //4.拿到系统的最匹配的配置
        val attributes = intArrayOf(
            EGL10.EGL_RED_SIZE, 8
            , EGL10.EGL_GREEN_SIZE, 8
            , EGL10.EGL_BLUE_SIZE, 8
            , EGL10.EGL_ALPHA_SIZE, 8
            , EGL10.EGL_DEPTH_SIZE, 8
            , EGL10.EGL_STENCIL_SIZE, 8
            , EGL10.EGL_RENDERABLE_TYPE, 4,
            EGL10.EGL_NONE
        )

        //4.1 得到最匹配配置的个数
        val configNums = IntArray(1)
        if (!mEgl!!.eglChooseConfig(mEglDisplay, attributes, null, 1, configNums)) {
            throw IllegalArgumentException("eglChooseConfig failed")
        }

        val configNum = configNums[0]

        if (configNum <= 0) {
            throw IllegalArgumentException(
                "No configs match configSpec"
            )
        }

        //4.2 得到最接近attributes的配置列表
        val config = arrayOfNulls<EGLConfig>(configNum)
        if (!mEgl!!.eglChooseConfig(mEglDisplay, attributes, config, configNum, configNums)) {
            throw IllegalArgumentException("eglChooseConfig failed")
        }

        //5.创建EGLContext对象
        val attributeList = intArrayOf(
            EGL14.EGL_CONTEXT_CLIENT_VERSION,
            2,
            EGL10.EGL_NONE
        )

        mEglContext = if (eglContext != null) {
            mEgl!!.eglCreateContext(mEglDisplay, config[0], eglContext, attributeList)
        } else {
            mEgl!!.eglCreateContext(mEglDisplay, config[0], EGL10.EGL_NO_CONTEXT, attributeList)
        }

        //6.创建EGLSurface对象
        mEglSurface = mEgl!!.eglCreateWindowSurface(mEglDisplay, config[0], surface, null)

        //7.Display和EGLSurface对象、mEglContext与当前线程进行绑定？binds context to the current rendering thread and to the draw and read surfaces
        if (!mEgl!!.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
            throw RuntimeException("eglMakeCurrent fail")
        }

    }

    public fun swapBuffers(): Boolean {
        mEgl?.let {
            return it.eglSwapBuffers(mEglDisplay, mEglSurface)
        }

        throw RuntimeException("egl is null")
    }

    public fun getEglContext(): EGLContext? {
        return mEglContext
    }

    public fun destroyEgl() {
        mEgl?.let {
            it.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT)
            it.eglDestroySurface(mEglDisplay, mEglSurface)
            mEglSurface = null

            it.eglDestroyContext(mEglDisplay, mEglContext)
            mEglContext = null

            it.eglTerminate(mEglDisplay)
            mEglDisplay = null
        }

        mEgl = null
    }
}