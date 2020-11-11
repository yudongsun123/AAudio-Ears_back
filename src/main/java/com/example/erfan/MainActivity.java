package com.example.erfan;

import android.Manifest;
import android.media.AudioManager;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    static {
        System.loadLibrary("Ears_back");
    }

    private boolean isWorking = false;
    private boolean mIsHeadsetOn = false;
    private AudioManager mAudioManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button start_ef = findViewById(R.id.btn_start_ef);
        Button stop_ef = findViewById(R.id.btn_stop_ef);
        start_ef.setOnClickListener(this);
        stop_ef.setOnClickListener(this);
        mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        requestPermission();
        initEF();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_start_ef:
                openEF();
                break;
            case R.id.btn_stop_ef:
                closeEF();
                break;
        }
    }

    private void openEF() {
        checkWiredHeadsetStatus();
        if (!mIsHeadsetOn) {
            Toast.makeText(this, "No wired headset detected", Toast.LENGTH_LONG).show();
            return;
        }

        if (!isWorking) {
            startEF();
            isWorking = true;
            Toast.makeText(this, "Start work", Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(this, "Already start work", Toast.LENGTH_SHORT).show();
        }
    }

    private void closeEF() {
        if (isWorking) {
            stopEF();
            isWorking = false;
            Toast.makeText(this, "Stop work", Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(this, "Have not started", Toast.LENGTH_SHORT).show();
        }
    }

    private void checkWiredHeadsetStatus() {
        if (mAudioManager != null) {
            mIsHeadsetOn = mAudioManager.isWiredHeadsetOn();
        }
    }

    private void requestPermission() {
        ActivityCompat.requestPermissions(this, new String[]{
                Manifest.permission.RECORD_AUDIO,
                Manifest.permission.MODIFY_AUDIO_SETTINGS
        }, 0);
    }

    private native void initEF();

    private native void startEF();

    private native void stopEF();

}
