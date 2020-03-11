package com.haha.record.opengl;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLES20;

import com.haha.record.R;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import static android.opengl.Matrix.orthoM;
import static android.opengl.Matrix.rotateM;

/**
 * 从fbo渲染到摄像头
 */
public class FboRender {

    private Context context;
    private final float[] projectionMatrix = new float[16];
    private int uMatrixLocation;

    private float[] vertexData = {
            -1f, -1f,
            1f, -1f,
            -1f, 1f,
            1f, 1f,

            //水印的占位顶点，具体还要计算水印尺寸得到
            0f, 0f,
            0f, 0f,
            0f, 0f,
            0f, 0f,
    };
    private FloatBuffer vertexBuffer;

    private float[] fragmentData = {
//            0f, 1f,
//            1f, 1f,
//            0f, 0f,
//            1f, 0f,

            0f, 0f,
            1f, 0f,
            0f, 1f,
            1f, 1f,
    };
    private FloatBuffer fragmentBuffer;
    //水印图片
    private Bitmap textImage;

    private int program;
    private int vPosition;
    private int fPosition;
    private int textImageTextureId;
    private int sampler;

    private int vboId;

    public FboRender(Context context) {
        this.context = context;
        //根据传入的文字和颜色以及尺寸生成一张对应的带文字的Bitmap
        textImage = ShaderUtil.crateTextImage("谈笑间樯橹灰飞烟灭", 50, "#ffeedd", "#00000000", 0);
        //水印宽高比
        float textImageRatio = 1.0f * textImage.getWidth() / textImage.getHeight();
        //设置水印实际高度为屏幕高的0.1f
        float textImageHeight = 0.1f;
        float textImageWidth = textImageHeight * textImageRatio;

        vertexData[8] = -textImageWidth / 2;
        vertexData[9] = -0.5f;

        vertexData[10] = textImageWidth / 2;
        vertexData[11] = -0.5f;

        vertexData[12] = -textImageWidth / 2;
        vertexData[13] = -0.6f;

        vertexData[14] = textImageWidth / 2;
        vertexData[15] = -0.6f;

        vertexBuffer = ByteBuffer.allocateDirect(vertexData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertexData);
        vertexBuffer.position(0);

        fragmentBuffer = ByteBuffer.allocateDirect(fragmentData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(fragmentData);
        fragmentBuffer.position(0);

    }

    public void onCreate() {
        String vertexSource;
        try {
            //支持透明绘制
            GLES20.glEnable(GLES20.GL_BLEND);
            GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
            vertexSource = ShaderUtil.readRawTExt(context, R.raw.vertex_shader_screen);
            //这里绑定的是显示到屏幕的Shader
            String fragmentSource = ShaderUtil.readRawTExt(context, R.raw.fragment_shader_screen);

            program = ShaderUtil.createProgram(vertexSource, fragmentSource);

            vPosition = GLES20.glGetAttribLocation(program, "av_Position");
            fPosition = GLES20.glGetAttribLocation(program, "af_Position");
            uMatrixLocation = GLES20.glGetUniformLocation(program, "u_Matrix");
            //    sampler = GLES20.glGetUniformLocation(program, "sTexture");

            int[] vbos = new int[1];
            GLES20.glGenBuffers(1, vbos, 0);
            vboId = vbos[0];

            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);
            GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, vertexData.length * 4 + fragmentData.length * 4, null, GLES20.GL_STATIC_DRAW);
            GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, 0, vertexData.length * 4, vertexBuffer);
            GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, vertexData.length * 4, fragmentData.length * 4, fragmentBuffer);
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

            textImageTextureId = ShaderUtil.loadBitmapTexture(textImage);
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public void onChange(int width, int height) {
        GLES20.glViewport(0, 0, width, height);
        //正交投影，将显示区域的偏小的边设为1，偏大的边看作大边和小边的比例
        final float aspectRatio = width > height ?
                (float) width / (float) height : (float) height / (float) width;
        if (width > height) {
            orthoM(projectionMatrix, 0, -aspectRatio, aspectRatio, -1f, 1f, -1f, 1f);
        } else {
            orthoM(projectionMatrix, 0, -1f, 1f, -aspectRatio, aspectRatio, -1f, 1f);
        }

        //   rotateM(projectionMatrix, 0, 180, 0, 0, 1);
        rotateM(projectionMatrix, 0, 180, 1, 0, 0);
    }

    public void onDraw(int textureId) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(1f, 0f, 0f, 1f);

        GLES20.glUseProgram(program);

        //  GLES20.glUniformMatrix4fv(uMatrixLocation, 1, false, projectionMatrix, 0);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);

        //绘制fbo纹理
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);

        GLES20.glEnableVertexAttribArray(vPosition);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8,
                0);

        GLES20.glEnableVertexAttribArray(fPosition);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8,
                vertexData.length * 4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
     //   GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

        //绘制水印纹理
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textImageTextureId);

        GLES20.glEnableVertexAttribArray(vPosition);
        //从第9个顶点数据开始
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8,
                32);

        GLES20.glEnableVertexAttribArray(fPosition);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8,
                vertexData.length * 4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
    }
}
