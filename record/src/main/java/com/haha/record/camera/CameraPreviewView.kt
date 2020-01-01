package com.haha.record.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.hardware.Camera
import android.util.AttributeSet
import android.util.Log
import android.view.WindowManager
import com.haha.record.egl.CustomGlSurfaceView

class CameraPreviewView : CustomGlSurfaceView {

    private var textureId: Int = -1
    private var cameraRender: CameraRender? = null
    private var customCamera: CustomCamera? = null
    private val cameraId = Camera.CameraInfo.CAMERA_FACING_BACK
   // private val cameraId = Camera.CameraInfo.CAMERA_FACING_FRONT

    constructor(ctx: Context) : super(ctx) {
        initView(ctx)
    }

    constructor(ctx: Context, attrs: AttributeSet) : super(ctx, attrs) {
        initView(ctx)
    }

    constructor(ctx: Context, attrs: AttributeSet, defStyleAttr: Int) : super(ctx, attrs, defStyleAttr) {
        initView(ctx)
    }

    private fun initView(ctx: Context) {
        cameraRender = CameraRender(ctx)
        customCamera = CustomCamera(ctx)
        setRender(cameraRender!!)
        previewAngle(ctx)
        cameraRender!!.onSurfaceCreatedListener = object : CameraRender.OnSurfaceCreatedListener {
            override fun onSurfadceCreated(surfaceTexture: SurfaceTexture, textureId: Int) {
                Log.e(TAG, "onSurfadceCreated")
                customCamera?.initCamera(surfaceTexture, cameraId)
                this@CameraPreviewView.textureId = textureId
            }
        }

    }

    fun onDestroy() {
        Log.e(TAG, "onSurfadceCreated")
        customCamera?.stopPreview()
    }

    fun previewAngle(context: Context) {
        val angle = (context.getSystemService(Context.WINDOW_SERVICE) as WindowManager).defaultDisplay.rotation
        cameraRender?.resetMatrix()

        Log.e(TAG, "previewAngle:$angle")

        val info = Camera.CameraInfo()
        Camera.getCameraInfo(cameraId, info)
        val angle1 = info.orientation
        cameraRender?.setAngle(angle1.toFloat(), 0f, 0f, 1f)
    }

    fun getTextureId(): Int? {
        return textureId
    }
}