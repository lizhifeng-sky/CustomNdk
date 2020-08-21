package com.android.ndk;

import android.app.Application;

/**
 * @author lizhifeng
 * @date 2020/8/20 18:44
 */
public class CustomeApplication extends Application {
    private static CustomeApplication customeApplication;

    @Override
    public void onCreate() {
        super.onCreate();
        customeApplication=this;
    }

    public static CustomeApplication getInstance(){
        return customeApplication;
    }
}
