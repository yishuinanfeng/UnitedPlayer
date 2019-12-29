package com.haha.record.opengl;

import android.content.Context;
import android.graphics.*;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;

public class WlShaderUtil {

    public static String readRawTExt(Context context, int rawId) throws IOException {
        InputStream inputStream = context.getResources().openRawResource(rawId);
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
        StringBuilder stringBuilder = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null) {
            stringBuilder.append(line).append("\n");
        }

        reader.close();
        return stringBuilder.toString();
    }


    public static int loadShader(int shaderType, String source) {
        int shader = GLES20.glCreateShader(shaderType);
        if (shader != 0) {
            GLES20.glShaderSource(shader, source);
            GLES20.glCompileShader(shader);
            int[] compile = new int[1];
            GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compile, 0);
            if (compile[0] != GLES20.GL_TRUE) {
                String info = GLES20.glGetShaderInfoLog(shader);
                Log.d("WlShaderUtil", "shader compile error:" + info);
                GLES20.glDeleteShader(shader);
                shader = 0;
            }
        }

        return shader;
    }

    public static int createProgram(String vertexSource, String fragmentSource) {
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexSource);
        if (vertexShader == 0) {
            return 0;
        }

        int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSource);
        if (fragmentShader == 0) {
            return 0;
        }

        int program = GLES20.glCreateProgram();
        if (program != 0) {
            GLES20.glAttachShader(program, vertexShader);
            GLES20.glAttachShader(program, fragmentShader);
            GLES20.glLinkProgram(program);

            int[] linkStatus = new int[1];
            GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, linkStatus, 0);
            if (linkStatus[0] != GLES20.GL_TRUE) {
                Log.d("WlShaderUtil", "link shader error");
                GLES20.glDeleteProgram(program);
                program = 0;
            }
        }
        Log.d("WlShaderUtil", "link shader success");
        return program;
    }

    /**
     * 根据传入的文字和颜色以及尺寸生成一张对应的带文字的Bitmap
     *
     * @param text
     * @param textSize
     * @param textColor
     * @param bgColor
     * @param padding
     * @return
     */
    public static Bitmap crateTextImage(String text, int textSize, String textColor, String bgColor, int padding) {
        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStyle(Paint.Style.FILL);
        paint.setTextSize(textSize);
        paint.setColor(Color.parseColor(textColor));
        //根据上面设置的文字内容和文字尺寸得到文字的宽度
        float textWidth = paint.measureText(text, 0, text.length());
        //根据上面设置的文字内容和文字尺寸得到文字的top和bottom（都是基于baseline）
        float top = paint.getFontMetrics().top;
        float bottom = paint.getFontMetrics().bottom;
        //根据文字的bottom、top、padding创建一个刚好能够填充文字的Bitmap
        Bitmap bm = Bitmap.createBitmap((int) (textWidth + 2 * padding), (int) (bottom - top + 2 * padding), Bitmap.Config.ARGB_4444);
        Canvas canvas = new Canvas(bm);
        canvas.drawColor(Color.parseColor(bgColor));
        //padding, -top + padding是基于当前的Canvas的坐标系指定的文字的baseline的最左边的点的坐标
        canvas.drawText(text, padding, -top + padding, paint);

        return bm;
    }

    /**
     * 根据传入的Bitmap生成一个新的纹理对象
     * @param bitmap
     * @return
     */
    public static int loadBitmapTexture(Bitmap bitmap) {
        int[] textureIds = new int[1];
        GLES20.glGenTextures(1, textureIds, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureIds[0]);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        //乘以4是因为每个像素有4个字节（ARGB_4444）
        ByteBuffer byteBuffer = ByteBuffer.allocate(bitmap.getWidth() * bitmap.getHeight() * 4);
        bitmap.copyPixelsToBuffer(byteBuffer);
        //将指针重置到开头
        //After this method returns, the current position of the buffer is
        //updated: the position is incremented by the number of elements written
        //in the buffer
        byteBuffer.flip();

        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, bitmap.getWidth(), bitmap.getHeight(), 0,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, byteBuffer);

        return textureIds[0];
    }

    public static int loadTexrute(int src, Context context)
    {
        int []textureIds = new int[1];
        GLES20.glGenTextures(1, textureIds, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureIds[0]);

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), src);
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
        bitmap.recycle();
        bitmap = null;
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        return  textureIds[0];

    }

}
