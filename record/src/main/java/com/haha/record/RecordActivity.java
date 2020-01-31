package com.haha.record;

import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import androidx.annotation.Keep;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.haha.record.camera.CameraPreviewView;
import com.haha.record.encodec.BaseMediaEncoder;
import com.haha.record.encodec.MediaEncoder;
import com.ywl5320.libmusic.WlMusic;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * 视频录制界面
 */
public class RecordActivity extends AppCompatActivity {
    private static String TAG = RecordActivity.class.getSimpleName();

    private CameraPreviewView cameraView;
    private Button btnRecord;

    private MediaEncoder mediaEncodec;
    private boolean finish = false;
    private SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyyMMddHHmmss", Locale.getDefault());

    static {
        System.loadLibrary("record-lib");
    }

    private void start() {
        finish = false;
        btnRecord.setText("正在录制...");
        btnRecord.setTextColor(Color.RED);
        //准备写入数据
        if (mediaEncodec == null) {
            Log.d("ywl5320", "textureid is " + cameraView.getTextureId());
            try {
                File parent = new File(ConfigKt.getVideoParentPath());
                if (!parent.exists()) {
                    parent.mkdirs();
                }
                String videoPath = ConfigKt.getVideoParentPath() + "/video" + simpleDateFormat.format(new Date())
                        + ".mp4";
                File file = new File(videoPath);
                if (!file.exists()) {
                    file.createNewFile();
                }

                mediaEncodec = new MediaEncoder(RecordActivity.this, cameraView.getTextureId());
                mediaEncodec.initEncoder(cameraView.getEglContext(), videoPath

                        , 720, 1280, 44100);
                mediaEncodec.setOnMediaInfoListener(new BaseMediaEncoder.OnMediaInfoListener() {
                    @Override
                    public void onMediaTime(long times) {
                        Log.d(TAG, "time is : " + times);
                    }
                });

                mediaEncodec.startRecord();

            } catch (IOException e) {
                Log.d(TAG, "IOException is : " + e);
            }
        }
        startRecord();
    }

    private void stop() {
        stopRecord();
        finish = true;
        //停止写入数据
        mediaEncodec.stopRecord();
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                btnRecord.setText("开始录制");
                btnRecord.setTextColor(Color.BLACK);
            }
        });
        mediaEncodec = null;
    }

    /**
     * 提供给native层调用(不可混淆)
     *
     * @param pcmData：麦克风获取到的音频数据
     */
    @Keep
    void onPcmDataInput(byte[] pcmData) {
        if (finish) {
            return;
        }
        Log.d("OpenSlDemo", "onPcmDataInput pcmData size:" + pcmData.length);
        if (mediaEncodec != null) {
            mediaEncodec.putPcmData(pcmData, pcmData.length);
        }
    }

    native void startRecord();

    native void stopRecord();

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        cameraView = findViewById(R.id.cameraview);
        btnRecord = findViewById(R.id.btn_record);

    }

    public void record(View view) {
        if (mediaEncodec == null) {
            start();
        } else {
            stop();
        }

    }
}
