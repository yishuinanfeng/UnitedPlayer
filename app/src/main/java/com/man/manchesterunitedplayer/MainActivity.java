package com.man.manchesterunitedplayer;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.haha.record.RecordActivity;
import com.man.manchesterunitedplayer.playlist.PlayListActivity;

import java.util.ArrayList;


public class MainActivity extends Activity {

    private static final int REQUEST_PLAY_PERMISSION = 275;
    private static final int REQUEST_RECORD_PERMISSION = 285;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.tv_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (ContextCompat.checkSelfPermission(MainActivity.this,
                        Manifest.permission.READ_EXTERNAL_STORAGE)
                        != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(MainActivity.this,
                            new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, REQUEST_PLAY_PERMISSION
                    );
                } else {
                    gotoPlayActivity();
                }
            }
        });

        findViewById(R.id.tv_record).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String[] permissions = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.CAMERA,
                        Manifest.permission.RECORD_AUDIO};

                boolean needRequestPermission = false;
                for (String permission : permissions) {
                    if (ContextCompat.checkSelfPermission(MainActivity.this,
                            permission)
                            != PackageManager.PERMISSION_GRANTED) {
                        needRequestPermission = true;
                    }
                }

                if (needRequestPermission) {
                    ActivityCompat.requestPermissions(MainActivity.this,
                            permissions, REQUEST_RECORD_PERMISSION
                    );
                } else {
                    gotoRecordActivity();
                }
            }
        });
    }

    private void gotoRecordActivity() {
        Intent intent = new Intent(MainActivity.this, RecordActivity.class);
        startActivity(intent);
    }

    private void gotoPlayActivity() {
        Intent intent = new Intent(MainActivity.this, PlayListActivity.class);
        startActivity(intent);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case REQUEST_PLAY_PERMISSION:
                for (int r : grantResults) {
                    if (r != PackageManager.PERMISSION_GRANTED) {
                        return;
                    }
                }

                gotoPlayActivity();
                break;

            case REQUEST_RECORD_PERMISSION:
                for (int r : grantResults) {
                    if (r != PackageManager.PERMISSION_GRANTED) {
                        return;
                    }
                }

                gotoRecordActivity();
                break;
        }

    }
}
