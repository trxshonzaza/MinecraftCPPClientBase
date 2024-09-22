#pragma once

#include <jni.h>

class VM {
public:
	JavaVM* jvm;
	JNIEnv* env;
	JNIEnv* renderEnv;
}vm;