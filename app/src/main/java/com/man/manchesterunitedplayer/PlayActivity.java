package com.man.manchesterunitedplayer;

import android.animation.ObjectAnimator;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.man.manchesterunitedplayer.bean.FilterBean;
import com.man.manchesterunitedplayer.playlist.FilterType;
import com.man.manchesterunitedplayer.util.MachineUtilKt;

import java.util.ArrayList;
import java.util.List;

import kotlin.Unit;
import kotlin.jvm.functions.Function1;

import static androidx.recyclerview.widget.RecyclerView.HORIZONTAL;
import static com.man.manchesterunitedplayer.FilterTypeKt.GRAY;
import static com.man.manchesterunitedplayer.FilterTypeKt.OPPOSITE_COLOR;
import static com.man.manchesterunitedplayer.FilterTypeKt.SCALE;
import static com.man.manchesterunitedplayer.FilterTypeKt.SHAKE;
import static com.man.manchesterunitedplayer.FilterTypeKt.SOUL;
import static com.man.manchesterunitedplayer.FilterTypeKt.SPLASH;
import static com.man.manchesterunitedplayer.KeyKt.KEY_INTENT_VIDEO_PATH;

/**
 * 播放界面
 */
public class PlayActivity extends Activity implements Runnable, SeekBar.OnSeekBarChangeListener {

    private static final String TAG = PlayActivity.class.getSimpleName();

    private SeekBar seekBar;
    private TextView tvFilter;
    private RecyclerView rvFilter;
    private boolean isFilterListShow;
    private ManchesterPlayer surfaceView;
    private boolean isSeekThreadStop;

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

        String videoPath = getIntent().getStringExtra(KEY_INTENT_VIDEO_PATH);

        surfaceView = findViewById(R.id.surfaceView);
        surfaceView.setVideoPath(videoPath);

        seekBar = findViewById(R.id.progressBar);
        tvFilter = findViewById(R.id.filter);
        rvFilter = findViewById(R.id.rv_filter);

        initFilterList();

        seekBar.setOnSeekBarChangeListener(this);
        tvFilter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                toggleFilterList();
            }
        });

        new Thread(this).start();
    }

    private void initFilterList() {
        final List<FilterBean> filterBeanList = new ArrayList<>();
        filterBeanList.add(new FilterBean(OPPOSITE_COLOR, "反色"));
        filterBeanList.add(new FilterBean(GRAY, "灰度图"));
        filterBeanList.add(new FilterBean(SPLASH, "闪白"));
        filterBeanList.add(new FilterBean(SCALE, "缩放"));
        filterBeanList.add(new FilterBean(SOUL, "灵魂出窍"));
        filterBeanList.add(new FilterBean(SHAKE, "抖动"));

        rvFilter.setAdapter(new FilterAdapter(this, filterBeanList, new Function1<Integer, Unit>() {
            @Override
            public Unit invoke(Integer position) {
                surfaceView.setFilterType(filterBeanList.get(position).getFilterType());
                return null;
            }
        }));
        rvFilter.setLayoutManager(new LinearLayoutManager(this, HORIZONTAL, false));
    }

    /**
     * 展开获取收起滤镜特效列表
     */
    private void toggleFilterList() {
        if (isFilterListShow) {

            Log.d(TAG,"close filterList");

            ObjectAnimator objectAnimator = ObjectAnimator
                    .ofFloat(rvFilter, "translationY", MachineUtilKt.dp2px(this, 80));
            objectAnimator.setDuration(500);
            objectAnimator.start();

            isFilterListShow = false;
        } else {
            Log.d(TAG,"open filterList");

            ObjectAnimator objectAnimator = ObjectAnimator
                    .ofFloat(rvFilter, "translationY",  0);
            objectAnimator.setDuration(500);
            objectAnimator.start();

            isFilterListShow = true;
        }
    }

    private native double getPlayPos();

    @Override
    public void run() {
        //轮询当前播放进度
        for (; ; ) {
            int pos = (int) (getPlayPos() * seekBar.getMax());
            if (pos == 0) {
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


    @Override
    protected void onDestroy() {
        super.onDestroy();
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

    @Override
    protected void onStop() {
        super.onStop();
        surfaceView.pausePlay();
        isSeekThreadStop = true;
    }

    @Override
    protected void onResume() {
        super.onResume();
        surfaceView.pausePlay();
        isSeekThreadStop = false;
    }

    private native void seek(double position);


}

