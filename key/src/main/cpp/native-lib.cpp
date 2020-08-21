#include <jni.h>
#include <string>
#include <iostream>

extern "C"
const char *KEY1 = "密钥1";
const char *KEY2 = "密钥2";
const char *KEY3 = "密钥3";
const char *UNKNOWN = "unknown";
const char *PACKAGE_NAME = "com.android.ndk";
//(签名的md5值自己可以写方法获取,或者用签名工具直接获取，
//一般对接微信sdk的时候也会要应用签名的MD5值)
const char *SIGN_MD5 = "C3DC5A2D4FAD89B0F71535E5B168FF8F";

jstring toMd5(JNIEnv *pEnv, _jbyteArray *source);

void toHexStr(const char *source, char *dest, jsize sourceLen);

bool getSignature(JNIEnv *pEnv);

std::string jstring2str(JNIEnv *env, jstring jstr);


extern "C" JNIEXPORT jstring JNICALL
Java_com_android_key_ReadKey_stringFromJNI(JNIEnv *env,
                                           jclass clazz,
                                           jstring key) {
    std::string hello = "Hello from C++11";
    if (getSignature(env)) {
        //校验通过
        if (jstring2str(env, key) == "key1") {
            return env->NewStringUTF(KEY1);
        } else if (jstring2str(env, key) == "key2") {
            return env->NewStringUTF(KEY2);
        } else if (jstring2str(env, key) == "key3") {
            return env->NewStringUTF(KEY3);
        }
    }
    return env->NewStringUTF(UNKNOWN);
}

jstring str2jstring(JNIEnv *env, const char *pat) {
    //定义java String类 strClass
    jclass strClass = (env)->FindClass("java/lang/String");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *) pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = (env)->NewStringUTF("GB2312");
    //将byte数组转换为java String,并输出
    return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
}

std::string jstring2str(JNIEnv *env, jstring jstr) {
    char *rtn = nullptr;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    auto barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

//获取Application实例
jobject getApplication(JNIEnv *env) {
    jobject application = NULL;
    //这里是你的Application的类路径
    jclass baseapplication_clz =
            env->FindClass("com/android/ndk/CustomeApplication");
    if (baseapplication_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(
                baseapplication_clz, "getInstance",
                "()Lcom/android/ndk/CustomeApplication;");
        if (currentApplication != NULL) {
            application = env->CallStaticObjectMethod(baseapplication_clz, currentApplication);
        }
        env->DeleteLocalRef(baseapplication_clz);
    }
    return application;
}


bool isRight = false;

//获取应用签名的MD5值并判断是否与本应用的一致
bool getSignature(JNIEnv *env) {
    if (!isRight) {
        //避免每次都进行校验浪费资源，只要第一次校验通过后，后边就不在进行校验
        jobject context = getApplication(env);
        // 获得Context类
        jclass cls = env->GetObjectClass(context);
        // 得到getPackageManager方法的ID
        jmethodID mid = env->GetMethodID(cls, "getPackageManager",
                                         "()Landroid/content/pm/PackageManager;");

        // 获得应用包的管理器
        jobject pm = env->CallObjectMethod(context, mid);

        // 得到getPackageName方法的ID
        mid = env->GetMethodID(cls, "getPackageName",
                               "()Ljava/lang/String;");
        // 获得当前应用包名
        jstring packageName = (jstring) env->CallObjectMethod(context, mid);
        const char *c_pack_name = env->GetStringUTFChars(packageName, NULL);

        // 比较包名,若不一致，直接return包名
        if (strcmp(c_pack_name, PACKAGE_NAME) != 0) {
            return false;
        }
        // 获得PackageManager类
        cls = env->GetObjectClass(pm);
        // 得到getPackageInfo方法的ID
        mid = env->GetMethodID(cls, "getPackageInfo",
                               "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
        // 获得应用包的信息
        jobject packageInfo = env->CallObjectMethod(pm, mid, packageName,
                                                    0x40); //GET_SIGNATURES = 64;
        // 获得PackageInfo 类
        cls = env->GetObjectClass(packageInfo);
        // 获得签名数组属性的ID
        jfieldID fid = env->GetFieldID(cls, "signatures", "[Landroid/content/pm/Signature;");
        // 得到签名数组
        jobjectArray signatures = (jobjectArray) env->GetObjectField(packageInfo, fid);
        // 得到签名
        jobject signature = env->GetObjectArrayElement(signatures, 0);

        // 获得Signature类
        cls = env->GetObjectClass(signature);
        mid = env->GetMethodID(cls, "toByteArray", "()[B");
        // 当前应用签名信息
        jbyteArray signatureByteArray = (jbyteArray) env->CallObjectMethod(signature, mid);
        //转成jstring
        jstring str = toMd5(env, signatureByteArray);
        char *c_msg = (char *) env->GetStringUTFChars(str, 0);
        isRight = strcmp(c_msg, SIGN_MD5) == 0;
        return isRight;
    }
    return isRight;
}

//md5
jstring toMd5(JNIEnv *env, jbyteArray source) {
    // MessageDigest
    jclass classMessageDigest = env->FindClass("java/security/MessageDigest");
    // MessageDigest.getInstance()
    jmethodID midGetInstance = env->GetStaticMethodID(classMessageDigest, "getInstance",
                                                      "(Ljava/lang/String;)Ljava/security/MessageDigest;");
    // MessageDigest object
    jobject objMessageDigest = env->CallStaticObjectMethod(classMessageDigest, midGetInstance,
                                                           env->NewStringUTF("md5"));

    jmethodID midUpdate = env->GetMethodID(classMessageDigest, "update", "([B)V");
    env->CallVoidMethod(objMessageDigest, midUpdate, source);

    // Digest
    jmethodID midDigest = env->GetMethodID(classMessageDigest, "digest", "()[B");
    jbyteArray objArraySign = (jbyteArray) env->CallObjectMethod(objMessageDigest, midDigest);

    jsize intArrayLength = env->GetArrayLength(objArraySign);
    jbyte *byte_array_elements = env->GetByteArrayElements(objArraySign, NULL);
    size_t length = (size_t) intArrayLength * 2 + 1;
    char *char_result = (char *) malloc(length);
    memset(char_result, 0, length);
    toHexStr((const char *) byte_array_elements, char_result, intArrayLength);
    // 在末尾补\0
    *(char_result + intArrayLength * 2) = '\0';
    jstring stringResult = env->NewStringUTF(char_result);
    // release
    env->ReleaseByteArrayElements(objArraySign, byte_array_elements, JNI_ABORT);
    // 指针
    free(char_result);
    return stringResult;
}

//转换为16进制字符串
void toHexStr(const char *source, char *dest, jsize sourceLen) {
    short i;
    char highByte, lowByte;
    for (i = 0; i < sourceLen; i++) {
        highByte = source[i] >> 4;
        lowByte = (char) (source[i] & 0x0f);
        highByte += 0x30;
        if (highByte > 0x39) {
            dest[i * 2] = (char) (highByte + 0x07);
        } else {
            dest[i * 2] = highByte;
        }
        lowByte += 0x30;
        if (lowByte > 0x39) {
            dest[i * 2 + 1] = (char) (lowByte + 0x07);
        } else {
            dest[i * 2 + 1] = lowByte;
        }
    }
}