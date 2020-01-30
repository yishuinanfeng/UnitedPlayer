package com.man.manchesterunitedplayer;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;

import com.man.manchesterunitedplayer.util.MachineUtilKt;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * 播放控件
 * SurfaceHolder.Callback作用是通知代码窗口的状态
 */
public class ManchesterPlayer extends GLSurfaceView implements SurfaceHolder.Callback, GLSurfaceView.Renderer {
    private static final String TAG = ManchesterPlayer.class.getSimpleName();
    private int filterType = -1;
    private String videoPath;

    public ManchesterPlayer(Context context) {
        super(context);
    }

    public ManchesterPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        setRenderer(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        initView(holder.getSurface(), filterType, getWidth(), getHeight());
        open(videoPath);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.d(TAG, "surfaceChanged");
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.d(TAG, "Renderer onSurfaceCreated");
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        Log.d(TAG, "Renderer onSurfaceChanged");
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        Log.d(TAG, "Renderer onDrawFrame");
    }

    public void setFilterType(int filterType) {
        setFilter(filterType);
        open(videoPath);
    }

    public void setVideoPath(String videoPath) {
        this.videoPath = videoPath;
    }

    public native void initView(Surface holderSurface, int filterType, int screenWidth, int screenHeight);

    public native void pausePlay();

    private native void open(String url);

    private native void setFilter(int filterType);

}
