package com.android.ndk;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.android.key.ReadKey;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final TextView tv = findViewById(R.id.sample_text);
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                tv.setText(ReadKey.stringFromJNI("key1"));
            }
        });
        final TextView tv2 = findViewById(R.id.sample_text_2);
        tv2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                tv2.setText(ReadKey.stringFromJNI("key2"));
            }
        });
        final TextView tv3 = findViewById(R.id.sample_text_3);
        tv3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                tv3.setText(ReadKey.stringFromJNI("key3"));
            }
        });
        final TextView tv4 = findViewById(R.id.sample_text_4);
        tv4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                tv4.setText(ReadKey.stringFromJNI("key1"));
            }
        });
    }
}
