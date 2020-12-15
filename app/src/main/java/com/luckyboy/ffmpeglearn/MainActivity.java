package com.luckyboy.ffmpeglearn;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.luckyboy.ffmpeglearn.listener.MediaErrorListener;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
//    static {
//        System.loadLibrary("native-lib");
//    }

    private static final String TAG = "MainActivity";

    String musicPath = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "ChinaY.mp3";

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.e(TAG, "onCreate: external " + musicPath);
        String musicInternalPath = getDataDir().getAbsolutePath()+File.separator+"ChinaY.mp3";
        Log.e(TAG, "onCreate: internal " + musicInternalPath);
        checkPermission();
        File musicFile = new File(musicInternalPath);
        if (!musicFile.exists()){
            Log.e(TAG, "onCreate: music doesn't exists");
            //return;
        }
        FFmpegPlayer fFmpegPlayer = new FFmpegPlayer();
        // http://mp32.9ku.com/upload/2016/02/17/669484.mp3
        String musicUrl = "http://mp32.9ku.com/upload/2016/02/17/669484_2.mp3";
        fFmpegPlayer.setErrorListener(new MediaErrorListener() {
            @Override
            public void onError(int code, String msg) {
                Log.e(TAG, "onError: "+code+" msg "+msg);
            }
        });
        fFmpegPlayer.playMedia(musicUrl);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();


    final int REQUEST_STORAGE_READ_ACCESS_PERMISSION = 100;

    private void checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            requestPermission(Manifest.permission.READ_EXTERNAL_STORAGE,
                    "需要打开读取权限",
                    REQUEST_STORAGE_READ_ACCESS_PERMISSION);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    protected void requestPermission(final String permission, String rationale, final int requestCode) {
        if (shouldShowRequestPermissionRationale(permission)) {
            Toast.makeText(this, rationale, Toast.LENGTH_SHORT).show();
        } else {
            requestPermissions(new String[]{permission}, requestCode);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case REQUEST_STORAGE_READ_ACCESS_PERMISSION:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // 正常读取
                    Log.e(TAG, "onRequestPermissionsResult: 获取权限成功");
                }
                break;
            default:
                super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

}

