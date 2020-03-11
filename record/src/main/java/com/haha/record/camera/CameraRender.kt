package com.haha.record.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.opengl.GLES11Ext
import android.opengl.GLES20.*
import android.opengl.Matrix
import android.util.Log
import com.haha.record.egl.CustomGlSurfaceView
import com.haha.record.R
import com.haha.record.opengl.FboRender
import com.haha.record.opengl.ShaderUtil
import com.haha.record.util.DisplayUtil
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer

/**
 * 从摄像头渲染到fbo。使用扩展纹理。
 */
class CameraRender(private val mContext: Context) : CustomGlSurfaceView.CustomRender,
        SurfaceTexture.OnFrameAvailableListener {

    private val TAG = "CameraRender"
    /**
     *  顶点着色器变换矩阵
     */
    private var uMatrixLocation: Int = 0
    /**
     *  顶点坐标数组
     */
    private val vertexData = floatArrayOf(-1f, -1f, 1f, -1f, -1f, 1f, 1f, 1f)
    private var vertexBuffer: FloatBuffer? = null
    /**
     * 纹理坐标数组
     */
    private val fragmentData = floatArrayOf(0f, 1f, 1f, 1f, 0f, 0f, 1f, 0f)
    private var fragmentBuffer: FloatBuffer? = null
    /**
     * 摄像头渲染的纹理id
     */
    private var cameraTextureId = 0
    /**
     * 摄像头渲染的SurfaceTexture（SurfaceTexture又和cameraTextureId绑定，使得摄像头数据可以渲染到cameraTextureId的纹理上）
     */
    private var surfaceTexture: SurfaceTexture? = null
    /**
     * 着色器程序id
     */
    private var program: Int = 0
    /**
     * 着色器顶点坐标字段location
     */
    private var vPosition: Int = 0
    /**
     * 着色器纹理坐标坐标字段location
     */
    private var fPosition: Int = 0

    private var vboId: Int = 0
    private var fboId: Int = 0
    /**
     * FBO的纹理id
     */
    private var fboTextureid: Int = 0
    /**
     * 使用一维数组表示4*4的二维数组
     */
    private val matrix = FloatArray(16)
    /**
     * 屏幕宽高
     */
    private var screenWidth: Int = 0
    private var screenHeight: Int = 0

    private var width: Int = 0
    private var height: Int = 0
    /**
     * 从fbo渲染到摄像头的渲染器
     */
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
        //获取顶点和片段着色器程序代码
        val vertexSource = ShaderUtil.readRawTExt(mContext, R.raw.vertex_shader)
        val fragmentSource = ShaderUtil.readRawTExt(mContext, R.raw.fragment_shader)
        //创建着色器程序
        program = ShaderUtil.createProgram(vertexSource, fragmentSource)
        //获取着色器中顶点坐标和纹理坐标字段的location
        vPosition = glGetAttribLocation(program, "av_Position")
        fPosition = glGetAttribLocation(program, "af_Position")
        //获取着色器中顶点坐标变换矩阵字段的location
        uMatrixLocation = glGetUniformLocation(program, "u_Matrix")

        //----------------------------初始化vbo----------------------------------
        val vbos = IntArray(1)
        glGenBuffers(1, vbos, 0)
        vboId = vbos[0]
        //该缓冲指定为vbo
        glBindBuffer(GL_ARRAY_BUFFER, vboId)
        //为vbo缓冲开辟指定大小的内存空间
        glBufferData(
                GL_ARRAY_BUFFER,
                vertexData.size * 4 + fragmentData.size * 4,
                null,
                GL_STATIC_DRAW
        )
        //vbo缓冲初始化数据（顶点坐标+纹理坐标）
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size * 4, vertexBuffer)
        glBufferSubData(GL_ARRAY_BUFFER, vertexData.size * 4, fragmentData.size * 4, fragmentBuffer)
        //解绑vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0)
        //---------------------------------解绑vbo----------------------------------

        //---------------------------------设置fbo帧缓冲---------------------------------------
        val fbos = IntArray(1)
        //todo 应该在不需要fbo时候删除fbo
        glGenBuffers(1, fbos, 0)
        fboId = fbos[0]
        //绑定了fbo,则后面在解绑fbo前的操作都是基于fbo
        //也可以把帧缓冲分开绑定到读或写目标上，分别使用GL_READ_FRAMEBUFFER或GL_DRAW_FRAMEBUFFER来做这件事。如果绑定到了GL_READ_FRAMEBUFFER，就能执行所有读取操作
        // ，像glReadPixels这样的函数使用了；绑定到GL_DRAW_FRAMEBUFFER上，就允许进行渲染、清空和其他的写入操作。大多数时候你不必分开用，
        // 通常把两个都绑定到GL_FRAMEBUFFER上就行
        glBindFramebuffer(GL_FRAMEBUFFER, fboId)

        //----为fbo创建纹理附件-----（除此还fbo可以使用渲染缓冲对象(Renderbuffer objects)）
        val textureIds = IntArray(1)
        glGenTextures(1, textureIds, 0)
        fboTextureid = textureIds[0]
        //绑定fbo的纹理
        glBindTexture(GL_TEXTURE_2D, fboTextureid)
        //环绕模式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
        //过滤模式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        //指定纹理目标的图像的缓冲尺寸、颜色类型格式、数据类型（最后参数传null，即先开辟空间和定义数据类型，后面才传具体数据。即对于这个纹理，我们只分配内存，而不去填充它）
        //纹理填充会在渲染到帧缓冲的时候去做
        //同样，要注意，我们不用关心环绕方式或者Mipmap，因为在大多数时候都不会需要它们的
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                screenWidth,
                screenHeight,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                null
        )
        //将纹理图像给FBO作为一个颜色附件(FBO提供了颜色缓冲、深度和模板缓冲),使得渲染到该纹理的数据会存放在帧缓冲中
        //一个附件就是一个内存地址，这个内存地址里面包含一个为帧缓冲准备的缓冲，它可以是个图像
        /**
         *  target：我们所创建的帧缓冲类型的目标（绘制、读取或两者都有）。
         *  attachment：我们所附加的附件的类型。现在我们附加的是一个颜色附件。需要注意，最后的那个0是暗示我们可以附加1个以上颜色的附件。我们会在后面的教程中谈到。
         *  textarget：你希望附加的纹理类型。
         *  texture：附加的实际纹理id。
         *  level：Mipmap level。我们设置为0
         */
        glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D,
                fboTextureid,
                0
        )
        //检查是否绑定成功
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Log.e(TAG, "fbo wrong")
        } else {
            Log.e(TAG, "fbo success")
        }
        //解绑纹理和FBO
        glBindTexture(GL_TEXTURE_2D, 0)
        glBindFramebuffer(GL_FRAMEBUFFER, 0)
        //---------------------------------解绑fbo缓冲---------------------------------------

        //--------------------------------------摄像头扩展纹理和SurfaceTexture绑定-----------------------------------
        val textureSeosId = IntArray(1)
        glGenTextures(1, textureSeosId, 0)
        //摄像头纹理
        cameraTextureId = textureSeosId[0]
        //摄像头纹理需要绑定的纹理目标为扩展纹理
        glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, cameraTextureId)
        //使用扩展纹理才可以在一个纹理上渲染yuv三个通道的数据（内部做了yuv和rgb的转换？）

        //环绕方式
        glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT)
        glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT)

        //过滤方式
        //放大的过滤
        glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
        //缩小的过滤
        glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        //SurfaceTexture和OpenGL纹理绑定(SurfaceTexture是OpenGL和Camera的桥梁)
        //使得Camera预览数据渲染到cameraTextureId的纹理，而该纹理已经和OpenGL的cameraTextureId绑定好了，所以会把摄像头数据渲染到OpenGL
        surfaceTexture = SurfaceTexture(cameraTextureId)
        surfaceTexture!!.setOnFrameAvailableListener(this)
        //这里会将surfaceTexture和纹理id交给外部的Camera进行绑定
        onSurfaceCreatedListener?.onSurfaceCreated(surfaceTexture!!, fboTextureid)

        glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0)
    }

    override fun onSurfaceChanged(width: Int, height: Int) {
        //  fboRender?.onChange(width, height)
        //   glViewport(0, 0, width, height)
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
        //所以就不用在这里绑定GL_TEXTURE_EXTERNAL_OES纹理目标（相当于绑定了cameraTextureId），因为之前绑定了摄像头，所以这里将摄像头预览画面数据渲染到cameraTextureId对应的纹理
        //updateTexImage may only be  called on the thread with the OpenGL ES context that contains the texture object 即绑定OpenGL Context的线程
        surfaceTexture?.updateTexImage()

        glClear(GL_COLOR_BUFFER_BIT)
        glClearColor(1f, 0f, 0f, 1f)

        glUseProgram(program)
        //渲染到fbo的纹理尺寸要和fbo一致
        glViewport(0, 0, screenWidth, screenHeight)
        //设置顶点着色器中的uMatrixLocation为matrix
        glUniformMatrix4fv(uMatrixLocation, 1, false, matrix, 0)
        //绑定fbo后，后面渲染到纹理的数据都会渲染到和fbo绑定的纹理上。这里渲染到surfaceTexture上的数据都会离屏渲染到fbo绑定的纹理上
        glBindFramebuffer(GL_FRAMEBUFFER, fboId)
        glBindBuffer(GL_ARRAY_BUFFER, vboId)

        glEnableVertexAttribArray(vPosition)
        glVertexAttribPointer(
                vPosition, 2, GL_FLOAT, false, 8,
                0
        )

        glEnableVertexAttribArray(fPosition)
        glVertexAttribPointer(
                fPosition, 2, GL_FLOAT, false, 8,
                vertexData.size * 4
        )

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)
        glBindTexture(GL_TEXTURE_2D, 0)
        glBindBuffer(GL_ARRAY_BUFFER, 0)
        //切换到屏幕的帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, 0)
        //将fbo中的数据显示到屏幕的尺寸要和实际屏幕尺寸一致
        fboRender?.onChange(width, height)
        //传入fbo绑定的纹理.前面都是绘制到cameraTextureId的纹理，现在将fbo中的数据渲染到屏幕
        fboRender?.onDraw(fboTextureid)
    }

    interface OnSurfaceCreatedListener {
        fun onSurfaceCreated(surfaceTexture: SurfaceTexture, textureId: Int)
    }

    override fun onFrameAvailable(p0: SurfaceTexture?) {

    }

    fun resetMatrix() {
        Matrix.setIdentityM(matrix, 0)
    }

    fun setAngle(angle: Float, x: Float, y: Float, z: Float) {
        //利用OpenGL进行图像的旋转，使得预览画面旋转到正确的方向（和手机屏幕一致的方向）
        Matrix.rotateM(matrix, 0, angle, x, y, z)
    }

    fun getTextureId(): Int {
        return fboTextureid
    }
}