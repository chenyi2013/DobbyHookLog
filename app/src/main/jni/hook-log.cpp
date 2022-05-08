//
// Created by JLGL on 2022/5/7.
//
#include <jni.h>
#include <string>
#include "dobby/include/dobby.h"
#include <android/log.h>
#include <jni.h>

JavaVM *g_jvm = NULL;
#define LOG_BUF_SIZE    1024
static const char *className = "com/jiliguala/hook/HookLog";

static int (*orig_log_print)(int prio, const char *tag, const char *fmt, ...);

/*
    线程执行的方法

    如果在 Native 层执行耗时操作 , 如下载文件 , 需要在线程中处理

    JNI 方法参数中的 JNIEnv 指针是不能跨线程使用的 , 在 主线程中调用 JNI 方法 , 其 JNIEnv 指针不能在子线程中使用

    如果在子线程中使用 JNIEnv 指针 , 需要使用 JavaVM 获取 指定线程的 JNIEnv 指针
        调用 JavaVM 的 AttachCurrentThread 可以获取本线程的 JNIEnv 指针
        注意最后还要将线程从 Java 虚拟机中剥离

    关于参数传递 :
        传递 int 类型  和 int * 类型 , 传递指针可以在 方法中修改 int 变量值 ;
        传递 int * 类型 和 int ** 类型 , 传递二维指针 可以在方法中修改 int * 一维指针值
        因此有些参数需要在方法中修改, 并且需要保存该修改状态 , 就需要将该变量的地址当做参数传入
            原来的普通变量 变成 指针变量 , 一维指针 变 二维指针

*/
void* threadRun(void *args){
    __android_log_print(ANDROID_LOG_INFO, "JNI_TAG", "threadRun");
    //JNIEnv 不能跨线程使用 , 这里需要先获取本线程的 JNIEnv
    JNIEnv *env;
    //将线程附加到 Java 虚拟机中 ( 注意后面对应剥离线程操作 )
    //  如果成功返回 0 , 如果失败 , 直接退出
    int attachResult = g_jvm->AttachCurrentThread(&env, 0);
    //将线程从 Java 虚拟机中剥离
    g_jvm->DetachCurrentThread();

    //注意这里一定要返回 0 , 否则执行到结尾会崩溃
    return 0;
}


static int my_libtest_log_print(int prio, const char *tag, const char *fmt, ...) {
    JNIEnv *env;
    //Attach主线程
    if (g_jvm->AttachCurrentThread(&env, NULL) != JNI_OK) {
        return 0;
    }
    //找到对应的类
    jmethodID mid;
    va_list ap;
    jclass cls = env->FindClass(className);
    jthrowable j_thr = env->ExceptionOccurred();
    if (j_thr) {
        //清除异常
        env->ExceptionClear();
        return 0;
    }
    mid = env->GetStaticMethodID(cls, "catchLog", "(ILjava/lang/String;Ljava/lang/String;)V");
    jstring tagName = env->NewStringUTF(tag);
    char *buf = new char[LOG_BUF_SIZE];
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);
    jstring msg = env->NewStringUTF(buf);
    int length = strlen(buf);
    env->CallStaticVoidMethod(cls, mid, prio, tagName, msg);
    env->DeleteLocalRef(cls);
    env->ReleaseStringUTFChars(msg, buf);
    env->DeleteLocalRef(msg);
    env->DeleteLocalRef(tagName);
    return length;
}

void threadDemoC(JNIEnv *env, jobject instance) {
    DobbyHook((void *) DobbySymbolResolver(NULL, "__android_log_print"),
              (void *) my_libtest_log_print, (void **) &orig_log_print);
}

static const JNINativeMethod methods[] = {
        {"init", "()V", (void *) threadDemoC}
};


int JNI_OnLoad(JavaVM *vm, void *reserved) {
    // 1 . 记录 Java 虚拟机指针
    g_jvm = vm;
    //获取 JNIEnv 指针
    JNIEnv *env = nullptr;
    int registerResult = vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (registerResult != JNI_OK) {
        return -1;
    }
    //进行动态注册
    jclass jclazz = env->FindClass(className);
    env->RegisterNatives(jclazz, methods, sizeof(methods) / sizeof(JNINativeMethod));
    return JNI_VERSION_1_6;
}