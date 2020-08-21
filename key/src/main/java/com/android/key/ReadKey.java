package com.android.key;

/**
 * @author lizhifeng
 * @date 2020/8/20 17:56
 */
public class ReadKey {
    static {
        System.loadLibrary("native-lib");
    }
    public static native String stringFromJNI(String key);

}
