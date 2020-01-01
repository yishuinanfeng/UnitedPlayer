package com.man.manchesterunitedplayer;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import com.haha.record.VideoActivity;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.tv_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this,PlayActivity.class);
                startActivity(intent);
            }
        });

        findViewById(R.id.tv_record).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, VideoActivity.class);
                startActivity(intent);
            }
        });
    }
}
