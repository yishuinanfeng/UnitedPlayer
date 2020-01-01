package com.man.manchesterunitedplayer;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.SeekBar;

/**
 * 播放界面
 */
public class PlayActivity extends Activity implements Runnable, SeekBar.OnSeekBarChangeListener {

    private SeekBar seekBar;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_play);

        findViewById(R.id.play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent();
                intent.setClass(PlayActivity.this, OpenUrlActivity.class);
                startActivity(intent);
            }
        });
        seekBar = findViewById(R.id.progressBar);
        seekBar.setOnSeekBarChangeListener(this);

        new Thread(this).start();
    }

    private native double getPlayPos();

    @Override
    public void run() {
        for (; ; ) {
            int pos = (int) (getPlayPos() * seekBar.getMax());
            if (pos == 0){
                continue;
            }
            seekBar.setProgress(pos);
            if (isSeekThreadStop) {
                break;
            }
            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private boolean isSeekThreadStop;

    @Override
    protected void onDestroy() {
        super.onDestroy();
        isSeekThreadStop = true;
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        seek((double) seekBar.getProgress() / (double) seekBar.getMax());
    }

    private native void seek(double position);
}

