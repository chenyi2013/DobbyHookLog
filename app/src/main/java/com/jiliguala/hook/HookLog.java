package com.jiliguala.hook;

import android.util.Log;

import androidx.annotation.Keep;

@Keep
public class HookLog {

    public static native void init();

    public static void catchLog(int c,String a, String b) {
        Log.i("kevin2", b+":"+c);
//        Log.println(c,"kevin",b);
    }
}
