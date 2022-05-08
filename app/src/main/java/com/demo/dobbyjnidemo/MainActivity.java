package com.demo.dobbyjnidemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.jiliguala.hook.HookLog;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("test");
        HookLog.init();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView textView = findViewById(R.id.text);
        textView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textView.setText(stringFromJNI());
            }
        });
    }

    public native String stringFromJNI();

}