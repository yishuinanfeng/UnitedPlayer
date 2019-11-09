package com.man.manchesterunitedplayer;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * 播放控件
 * SurfaceHolder.Callback作用是通知代码窗口的状态
 */
public class ManchesterPlayer extends GLSurfaceView implements SurfaceHolder.Callback, GLSurfaceView.Renderer {
    private static final String TAG = ManchesterPlayer.class.getSimpleName();

    public ManchesterPlayer(Context context) {
        super(context);
    }

    public ManchesterPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        setRenderer(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG,"surfaceCreated");
        initView(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG,"surfaceDestroyed");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.d(TAG,"surfaceChanged");
    }

    public native void initView(Object surface);

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.d(TAG,"Renderer onSurfaceCreated");
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        Log.d(TAG,"Renderer onSurfaceChanged");
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        Log.d(TAG,"Renderer onDrawFrame");
    }
}
