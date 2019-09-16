package com.man.manchesterunitedplayer;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

/**
 * 播放控件
 * SurfaceHolder.Callback作用是通知代码窗口的状态
 */
public class ManchesterPlayer extends GLSurfaceView implements SurfaceHolder.Callback {

    public ManchesterPlayer(Context context) {
        super(context);
    }

    public ManchesterPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        initView(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {

    }

    public native void initView(Object surface);
}
