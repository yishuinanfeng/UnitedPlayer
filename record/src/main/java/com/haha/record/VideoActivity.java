package com.haha.record;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.haha.record.camera.CameraPreviewView;
import com.haha.record.encodec.BaseMediaEncoder;
import com.haha.record.encodec.MediaEncoder;
import com.ywl5320.libmusic.WlMusic;
import com.ywl5320.listener.OnCompleteListener;
import com.ywl5320.listener.OnPreparedListener;
import com.ywl5320.listener.OnShowPcmDataListener;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * 视频录制界面
 */
public class VideoActivity extends AppCompatActivity {
    private static String TAG = VideoActivity.class.getSimpleName();

    private CameraPreviewView cameraView;
    private Button btnRecord;

    private MediaEncoder mediaEncodec;
    //提供音乐
    private WlMusic wlMusic;
    private boolean finish = false;
    private SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyyMMddHHmmss", Locale.getDefault());

    static {
        System.loadLibrary("record-lib");
    }

    private void start() {
        finish = false;
        btnRecord.setText("正在录制");
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

                mediaEncodec = new MediaEncoder(VideoActivity.this, cameraView.getTextureId());
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
            }
        });
        mediaEncodec = null;
    }

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

//        wlMusic = WlMusic.getInstance();
//        wlMusic.setCallBackPcmData(true);
//
//        wlMusic.setOnPreparedListener(new OnPreparedListener() {
//            @Override
//            public void onPrepared() {
//                wlMusic.playCutAudio(39, 60);
//            }
//        });
//
//        wlMusic.setOnCompleteListener(new OnCompleteListener() {
//            @Override
//            public void onComplete() {
//                mediaEncodec.stopRecord();
//                runOnUiThread(new Runnable() {
//                    @Override
//                    public void run() {
//                        btnRecord.setText("开始录制");
//                    }
//                });
//                mediaEncodec = null;
//            }
//        });
//
//        wlMusic.setOnShowPcmDataListener(new OnShowPcmDataListener() {
//            @Override
//            public void onPcmInfo(int samplerate, int bit, int channels) {
//                if (mediaEncodec == null) {
//                    Log.d("ywl5320", "textureid is " + cameraView.getTextureId());
//                    mediaEncodec = new MediaEncoder(VideoActivity.this, cameraView.getTextureId());
//                    mediaEncodec.initEncoder(cameraView.getEglContext(),
//                            Environment.getExternalStorageDirectory().getAbsolutePath() + "/manlian.mp4"
//                            , 720, 1280, samplerate);
//                    mediaEncodec.setOnMediaInfoListener(new BaseMediaEncoder.OnMediaInfoListener() {
//                        @Override
//                        public void onMediaTime(long times) {
//                            Log.d(TAG, "time is : " + times);
//                        }
//                    });
//
//                    mediaEncodec.startRecord();
//                }
//
//
//            }
//
//            @Override
//            public void onPcmData(byte[] pcmdata, int size, long clock) {
//                if (mediaEncodec != null) {
//                    mediaEncodec.putPcmData(pcmdata, size);
//                }
//            }
//
//        });
    }

    public void record(View view) {
        //因为停止录制的时候会将wlMediaEncodec置为空，所以可以根据wlMediaEncodec是否为空判断是否是正在录制状态
//        if (mediaEncodec == null) {
//            wlMusic.setSource(Environment.getExternalStorageDirectory().getAbsolutePath() + "/haikuotiankong.mp3");
//            wlMusic.prePared();
//            btnRecord.setText("正在录制");
//        } else {
//            mediaEncodec.stopRecord();
//            btnRecord.setText("开始录制");
//            mediaEncodec = null;
//            wlMusic.stop();
//        }

        if (mediaEncodec == null) {
            start();
        } else {
            stop();
        }

    }
}
