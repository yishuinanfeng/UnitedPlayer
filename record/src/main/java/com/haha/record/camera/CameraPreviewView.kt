package com.haha.record.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.hardware.Camera
import android.util.AttributeSet
import android.util.Log
import android.view.WindowManager
import com.haha.record.egl.CustomGlSurfaceView

/**
 * 专门用来给摄像头预览的GlSurfaceView
 */
class CameraPreviewView : CustomGlSurfaceView {

    private var textureId: Int = -1
    /**
     * 当前GlSurfaceView的Render
     */
    private var cameraRender: CameraRender? = null
    /**
     * 摄像头包装类对象
     */
    private var customCamera: CustomCamera? = null
    /**
     * 后置摄像头
     */
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
            override fun onSurfaceCreated(surfaceTexture: SurfaceTexture, textureId: Int) {
                //GLSurface创建完之后，和OpenGl纹理关联的surfaceTexture再和摄像头绑定
                Log.e(TAG, "onSurfaceCreated")
                customCamera?.initCamera(surfaceTexture, cameraId)
                //纹理id保存在CameraPreviewView，随时提供给录制线程渲染使用
                this@CameraPreviewView.textureId = textureId
            }
        }

    }

    fun onDestroy() {
        customCamera?.stopPreview()
    }

    /**
     * 预览角度
     */
    private fun previewAngle(context: Context) {
        //获得屏幕方向：当前屏幕相对于（竖直？）自然握持方向的角度（逆时针）
        val angle = (context.getSystemService(Context.WINDOW_SERVICE) as WindowManager).defaultDisplay.rotation
        cameraRender?.resetMatrix()

        Log.e(TAG, "previewAngle:$angle")

        val info = Camera.CameraInfo()
        Camera.getCameraInfo(cameraId, info)

        //这里就是摄像头传感器和手机屏幕自然握持方向的角度差？（摄像头传感器和需要转多少度才和手机屏幕自然握持方向一致）
        // 对于后置摄像头来说，要从手机后面看才是顺时针需要旋转的角度？
        val angle1 = info.orientation
        //xyz中指定为1的就是围绕此轴旋转（内部是利用OpenGL顶点着色器进行图像的旋转）
        // setDisplayOrientation仅仅改变预览方向，不会改变录制方向
        //todo 这里应该根据Camera官方文档去设置方向角度
        cameraRender?.setAngle(angle1.toFloat(), 0f, 0f, 1f)
    }

    fun getTextureId(): Int? {
        return textureId
    }
}