package com.haha.record.egl

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import java.lang.ref.WeakReference
import javax.microedition.khronos.egl.EGLContext

/**
* @date 2020/3/8
* @user yanyinan
* @description 自定义的GlSurfaceView，主要用于将OpenGL数据同时渲染到多个纹理上
**/
open class CustomGlSurfaceView : SurfaceView, SurfaceHolder.Callback {

    protected val TAG = this.javaClass.simpleName

    private var mSurface: Surface? = null
    private var mEglContext: EGLContext? = null
    private var eglThread: EglThread? = null
    private var mRender: CustomRender? = null

    private var mRenderMode = RENDERMODE_CONTINUOUSLY

    companion object {
        const val RENDERMODE_WHEN_DIRTY = 0
        const val RENDERMODE_CONTINUOUSLY = 1
    }

    constructor(ctx: Context) : super(ctx) {
        init()
    }

    constructor(ctx: Context, attrs: AttributeSet) : super(ctx, attrs) {
        init()
    }

    constructor(ctx: Context, attrs: AttributeSet, defStyleAttr: Int) : super(ctx, attrs, defStyleAttr) {
        init()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        Log.d("CustomGlSurfaceView", "surfaceChanged")
        eglThread?.width = width
        eglThread?.height = height
        eglThread?.isChange = true
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        Log.d("CustomGlSurfaceView", "surfaceDestroyed")
        eglThread?.onDestroy()
        eglThread = null
        mSurface = null
        mEglContext = null
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        Log.d("CustomGlSurfaceView", "surfaceCreated")
        if (mSurface == null) {
            mSurface = holder.surface
        }
        eglThread = EglThread(WeakReference(this))
        eglThread!!.isCreate = true
        eglThread!!.start()
    }

    fun setSurfaceAndEglContext(surface: Surface?, eglContext: EGLContext?) {
        mSurface = surface
        mEglContext = eglContext
    }

    fun setRender(render: CustomRender) {
        mRender = render
    }

    fun setRenderMode(mode: Int) {
        mRender?.let {
            mRenderMode = mode
        }
    }

    private fun init() {
        holder.addCallback(this)
    }

    interface CustomRender {
        fun onSurfaceCreated()
        fun onSurfaceChanged(width: Int, height: Int)
        fun onDrawFrame()
    }

    fun requestRender() {
        eglThread?.requestRender()
    }

    class EglThread(private val surfaceViewReference: WeakReference<CustomGlSurfaceView>) : Thread() {
        private var eglHelper: EglHelper? = null

        private var isExit = false
        private var isStart = false
        private var lock: Object? = null

        var isChange = false
        var isCreate = false
        var width: Int = 0
        var height: Int = 0

        override fun run() {
            Log.d("CustomGlSurfaceView", "MediaRenderThread run")
            eglHelper = EglHelper()
            lock = Object()
            surfaceViewReference.get()?.let {
                if (it.mSurface == null) {
                    return
                }
                eglHelper!!.initEgl(it.mSurface!!, it.mEglContext)
            }

            while (true) {
                if (isExit) {
                    release()
                    break
                }

                //已经开始才进行画面刷新
                if (isStart) {
                    surfaceViewReference.get()?.let {
                        when {
                            it.mRenderMode == RENDERMODE_WHEN_DIRTY -> synchronized(lock!!) {
                                //
                                lock!!.wait()
                            }
                            it.mRenderMode == RENDERMODE_CONTINUOUSLY -> sleep(60)
                            else -> throw RuntimeException("mRenderMode is wrong value")
                        }
                    }
                }

                onCreate()
                onChange(width, height)
                onDraw()

                isStart = true
            }
        }

        private fun onDraw() {
            surfaceViewReference.get()?.let {
                if (it.mRender != null && eglHelper != null) {
                    it.mRender!!.onDrawFrame()
                    if (!isStart) {
                        it.mRender!!.onDrawFrame()
                    }

                    eglHelper!!.swapBuffers()
                }
            }
        }


        /**
         * 手动刷新
         */
         fun requestRender() {
            lock?.let {
                synchronized(it) {
                    it.notifyAll()
                }
            }
        }

        fun onDestroy() {
            isExit = true
            //??
            requestRender()
        }

        fun getEglContext(): EGLContext? {
            return eglHelper?.getEglContext()
        }

        private fun onChange(width: Int, height: Int) {
            surfaceViewReference.get()?.let {
                if (isChange && it.mRender != null) {
                    isChange = false
                    it.mRender!!.onSurfaceChanged(width, height)
                }
            }

        }

        private fun onCreate() {
            surfaceViewReference.get()?.let {
                if (isCreate && it.mRender != null) {
                    isCreate = false
                    it.mRender!!.onSurfaceCreated()
                }

            }
        }

        private fun release() {
            eglHelper?.destroyEgl()
            eglHelper = null
            lock = null
        }
    }

    fun getEglContext():EGLContext?{
        return eglThread?.getEglContext()
    }

}