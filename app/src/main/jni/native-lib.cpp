//
// Created by JLGL on 2022/5/8.
//
#include <jni.h>
#include <string>
#include "dobby/include/dobby.h"
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOG_TAG "kevin"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_demo_dobbyjnidemo_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//指向原函数地址 方法名价格*oragin
static jstring (*orgin_Java_com_demo_dobbyjnidemo_MainActivity_stringFromJNI)(JNIEnv* env,jobject /* this */);

// 代替的方法
extern "C" JNIEXPORT jstring JNICALL
new_Java_com_demo_dobbyjnidemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "我被hook修改了";
    LOGD("1xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    return env->NewStringUTF(hello.c_str());
}

__attribute__((constructor)) static void ctor() {
    // 构造函数 静态插入hook调用
    // 原函数名
    // 新函数地址
    // 旧函数地址
    DobbyHook((void *) DobbySymbolResolver(NULL, "Java_com_demo_dobbyjnidemo_MainActivity_stringFromJNI"), (void *) new_Java_com_demo_dobbyjnidemo_MainActivity_stringFromJNI,(void **) &orgin_Java_com_demo_dobbyjnidemo_MainActivity_stringFromJNI);
}