package com.haha.record.encodec

import android.content.Context
import android.graphics.Bitmap
import android.opengl.GLES20
import android.opengl.Matrix.orthoM
import android.opengl.Matrix.rotateM
import com.haha.record.egl.CustomGlSurfaceView
import com.haha.record.R
import com.haha.record.opengl.WlShaderUtil
import java.io.IOException
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer

/**
 * 将摄像头预览数据渲染到MediaCode的surface中进行编码
 */
class MediaEncodeRender(val context: Context, private val textureId: Int) : CustomGlSurfaceView.CustomRender {

    private val projectionMatrix = FloatArray(16)

    private val vertexData = floatArrayOf(
            -1f, -1f, 1f, -1f, -1f, 1f, 1f, 1f,
            //水印的占位顶点，具体还要计算水印尺寸得到
            0f, 0f,
            0f, 0f,
            0f, 0f,
            0f, 0f
    )
    private var vertexBuffer: FloatBuffer? = null

    private val fragmentData = floatArrayOf(0f, 0f, 1f, 0f, 0f, 1f, 1f, 1f)
    private var fragmentBuffer: FloatBuffer? = null

    private var program: Int = 0
    private var vPosition: Int = 0
    private var fPosition: Int = 0
    private var vboId: Int = 0

    //水印图片
    private var textImage: Bitmap? = null
    private var textImageTextureId: Int = 0

    init {

        textImage = WlShaderUtil.crateTextImage("谈笑间樯橹灰飞烟灭", 50, "#ffeedd", "#00000000", 0)
        //水印宽高比
        val textImageRatio = 1.0f * textImage!!.width / textImage!!.height
        //设置水印实际高度为屏幕高的0.1f
        val textImageHeight = 0.1f
        val textImageWidth = textImageHeight * textImageRatio

        vertexData[8] = -textImageWidth / 2
        vertexData[9] = -0.5f

        vertexData[10] = textImageWidth / 2
        vertexData[11] = -0.5f

        vertexData[12] = -textImageWidth / 2
        vertexData[13] = -0.6f

        vertexData[14] = textImageWidth / 2
        vertexData[15] = -0.6f

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
        val vertexSource: String
        try {
            //支持透明绘制
            GLES20.glEnable(GLES20.GL_BLEND)
            GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA)

            vertexSource = WlShaderUtil.readRawTExt(context, R.raw.vertex_shader_screen)
            //这里绑定的是显示到屏幕的Shader
            val fragmentSource = WlShaderUtil.readRawTExt(context, R.raw.fragment_shader_screen)

            program = WlShaderUtil.createProgram(vertexSource, fragmentSource)

            vPosition = GLES20.glGetAttribLocation(program, "av_Position")
            fPosition = GLES20.glGetAttribLocation(program, "af_Position")
            //  uMatrixLocation = GLES20.glGetUniformLocation(program, "u_Matrix")
            //    sampler = GLES20.glGetUniformLocation(program, "sTexture");

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

            textImageTextureId = WlShaderUtil.loadBitmapTexture(textImage)

        } catch (e: IOException) {
            e.printStackTrace()
        }

    }

    override fun onSurfaceChanged(width: Int, height: Int) {
        GLES20.glViewport(0, 0, width, height)
        //正交投影，将显示区域的偏小的边设为1，偏大的边看作大边和小边的比例
        val aspectRatio = if (width > height)
            width.toFloat() / height.toFloat()
        else
            height.toFloat() / width.toFloat()
        if (width > height) {
            orthoM(projectionMatrix, 0, -aspectRatio, aspectRatio, -1f, 1f, -1f, 1f)
        } else {
            orthoM(projectionMatrix, 0, -1f, 1f, -aspectRatio, aspectRatio, -1f, 1f)
        }

        //   rotateM(projectionMatrix, 0, 180, 0, 0, 1);
        rotateM(projectionMatrix, 0, 180f, 1f, 0f, 0f)
    }

    override fun onDrawFrame() {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
        GLES20.glClearColor(1f, 0f, 0f, 1f)

        GLES20.glUseProgram(program)

        //  GLES20.glUniformMatrix4fv(uMatrixLocation, 1, false, projectionMatrix, 0);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId)

        //绘制摄像头预览的数据
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId)

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

        //绘制水印
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textImageTextureId)

        GLES20.glEnableVertexAttribArray(vPosition)
        GLES20.glVertexAttribPointer(
                vPosition, 2, GLES20.GL_FLOAT, false, 8,
                32
        )

        GLES20.glEnableVertexAttribArray(fPosition)
        GLES20.glVertexAttribPointer(
                fPosition, 2, GLES20.GL_FLOAT, false, 8,
                vertexData.size * 4
        )

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4)

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)
    }
}