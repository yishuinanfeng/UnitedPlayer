package com.haha.record.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.opengl.GLES11Ext
import android.opengl.GLES20
import android.opengl.GLES20.*
import android.opengl.Matrix
import android.util.Log
import com.haha.record.egl.CustomGlSurfaceView
import com.haha.record.R
import com.haha.record.opengl.FboRender
import com.haha.record.opengl.WlShaderUtil
import com.haha.record.util.DisplayUtil
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer

/**
 * 从摄像头渲染到fbo。使用扩展纹理。
 */
class CameraRender(private val mContext: Context) : CustomGlSurfaceView.CustomRender,
    SurfaceTexture.OnFrameAvailableListener {

    private var uMatrixLocation: Int = 0
    private val TAG = "CameraRender"

    private val vertexData = floatArrayOf(-1f, -1f, 1f, -1f, -1f, 1f, 1f, 1f)
    private var vertexBuffer: FloatBuffer? = null

    private val fragmentData = floatArrayOf(0f, 1f, 1f, 1f, 0f, 0f, 1f, 0f)
    private var fragmentBuffer: FloatBuffer? = null

    private var cameraTextureId = 0
    private var surfaceTexture: SurfaceTexture? = null
    private var program: Int = 0
    private var vPosition: Int = 0
    private var fPosition: Int = 0
    private var vboId: Int = 0
    private var fboId: Int = 0

    private var fboTextureid: Int = 0
    //使用一维数组表示4*4的二维数组
    private val matrix = FloatArray(16)

    private var screenWidth: Int = 0
    private var screenHeight: Int = 0
    private var width: Int = 0
    private var height: Int = 0

    private var fboRender: FboRender? = null

    var onSurfaceCreatedListener: OnSurfaceCreatedListener? = null

    init {
        screenWidth = DisplayUtil.getScreenWidth(mContext)
        screenHeight = DisplayUtil.getScreenHeight(mContext)

        fboRender = FboRender(mContext)
        //将数组数据存放在Native层
        vertexBuffer = ByteBuffer.allocateDirect(vertexData.size * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
            .put(vertexData)
        vertexBuffer!!.position(0)

        fragmentBuffer = ByteBuffer.allocateDirect(fragmentData.size * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
            .put(fragmentData)
        fragmentBuffer!!.position(0)

    }

    override fun onSurfaceCreated() {
        fboRender?.onCreate()

        Log.e(TAG, "onSurfaceCreated")

        val vertexSource = WlShaderUtil.readRawTExt(mContext, R.raw.vertex_shader)
        val fragmentSource = WlShaderUtil.readRawTExt(mContext, R.raw.fragment_shader)

        program = WlShaderUtil.createProgram(vertexSource, fragmentSource)
        vPosition = GLES20.glGetAttribLocation(program, "av_Position")
        fPosition = GLES20.glGetAttribLocation(program, "af_Position")
        uMatrixLocation = GLES20.glGetUniformLocation(program, "u_Matrix")
        //初始化vbo数据
        val vbos = IntArray(1)
        GLES20.glGenBuffers(1, vbos, 0)
        vboId = vbos[0]

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId)
        GLES20.glBufferData(
            GLES20.GL_ARRAY_BUFFER,
            vertexData.size * 4 + fragmentData.size * 4,
            null,
            GLES20.GL_STATIC_DRAW
        )
        GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, 0, vertexData.size * 4, vertexBuffer)
        GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, vertexData.size * 4, fragmentData.size * 4, fragmentBuffer)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)
        //创建fbo
        val fbos = IntArray(1)
        GLES20.glGenBuffers(1, fbos, 0)
        fboId = fbos[0]
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId)
        //创建fbo的纹理
        val textureIds = IntArray(1)
        glGenTextures(1, textureIds, 0)
        fboTextureid = textureIds[0]

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, fboTextureid)

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)

        GLES20.glTexImage2D(
            GLES20.GL_TEXTURE_2D,
            0,
            GLES20.GL_RGBA,
            screenWidth,
            screenHeight,
            0,
            GLES20.GL_RGBA,
            GLES20.GL_UNSIGNED_BYTE,
            null
        )
        GLES20.glFramebufferTexture2D(
            GLES20.GL_FRAMEBUFFER,
            GLES20.GL_COLOR_ATTACHMENT0,
            GLES20.GL_TEXTURE_2D,
            fboTextureid,
            0
        )
        if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
            Log.e(TAG, "fbo wrong")
        } else {
            Log.e(TAG, "fbo success")
        }
        glBindTexture(GLES20.GL_TEXTURE_2D, 0)
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)

        //摄像头纹理
        val textureSeosId = IntArray(1)
        glGenTextures(1, textureSeosId, 0)
        cameraTextureId = textureSeosId[0]
        //摄像头纹理需要绑定的纹理目标为扩展纹理
        glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, cameraTextureId)
        //使用扩展纹理才可以在一个纹理上渲染yuv三个通道的数据
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_MIRRORED_REPEAT)
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_MIRRORED_REPEAT)
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)
        //SurfaceTexture和OpenGL纹理绑定(SurfaceTexture是OpenGL和Camera的桥梁)
        //使得Camera预览数据渲染到cameraTextureId的纹理，而该纹理已经和OpenGL的cameraTextureId绑定好了，所以渲染到OpenGL
        surfaceTexture = SurfaceTexture(cameraTextureId)
        surfaceTexture!!.setOnFrameAvailableListener(this)
        //这里会将surfaceTexture交给外部的Camera进行绑定
        onSurfaceCreatedListener?.onSurfadceCreated(surfaceTexture!!, fboTextureid)

        glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0)
    }

    override fun onSurfaceChanged(width: Int, height: Int) {
        //  fboRender?.onChange(width, height)
        //   GLES20.glViewport(0, 0, width, height)
        //    Matrix.rotateM(matrix, 0, 90f, 0f, 0f, 1f)

        Log.e(TAG, "onSurfaceChanged width:$width,height:$height")

        this.width = width
        this.height = height
    }

    /**
     * 这里先绘制到和surfaceTexture绑定的cameraTextureId，再讲纹理输出到和fbo绑定的fboTextureid，然后fboTextureid传给fboRender绘制
     */
    override fun onDrawFrame() {

        //    Log.e(TAG, "onDrawFrame")
        //It will implicitly bind its texture to the GL_TEXTURE_EXTERNAL_OES texture target
        //所以就不用在这里绑定GL_TEXTURE_EXTERNAL_OES纹理目标（相当于绑定了cameraTextureId），即绑定了摄像头的预览画面
        surfaceTexture?.updateTexImage()

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
        GLES20.glClearColor(1f, 0f, 0f, 1f)

        GLES20.glUseProgram(program)
        //渲染到fbo的纹理尺寸要和fbo一致
        GLES20.glViewport(0, 0, screenWidth, screenHeight)
        //各个参数含义
        glUniformMatrix4fv(uMatrixLocation, 1, false, matrix, 0)
        //绑定fbo后，后面渲染到纹理的数据都会渲染到和fbo绑定的纹理上。这里渲染到surfaceTexture上的数据都会离屏渲染到fbo绑定的纹理上
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId)

        GLES20.glEnableVertexAttribArray(vPosition)
        GLES20.glVertexAttribPointer(
            vPosition, 2, GLES20.GL_FLOAT, false, 8,
            0
        )

        GLES20.glEnableVertexAttribArray(fPosition)
        GLES20.glVertexAttribPointer(
            fPosition, 2, GLES20.GL_FLOAT, false, 8,
            vertexData.size * 4
        )

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)
        //切换到屏幕的帧缓冲
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)
        //将fbo中的数据显示到屏幕的尺寸要和实际屏幕尺寸一致
        fboRender?.onChange(width, height)
        //传入fbo绑定的纹理.前面都是绘制到cameraTextureId的纹理，现在将fbo中的数据渲染到屏幕
        fboRender?.onDraw(fboTextureid)
    }

    interface OnSurfaceCreatedListener {
        fun onSurfadceCreated(surfaceTexture: SurfaceTexture, textureId: Int)
    }

    override fun onFrameAvailable(p0: SurfaceTexture?) {

    }

    fun resetMatrix() {
        Matrix.setIdentityM(matrix, 0)
    }

    fun setAngle(angle: Float, x: Float, y: Float, z: Float) {
        Matrix.rotateM(matrix, 0, angle, x, y, z)
    }

    fun getTextureId(): Int {
        return fboTextureid
    }
}