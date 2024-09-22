#pragma once

#include "VM.h"
#include "VM.h"
#include "AxisAlignedBB.h"
#include <jni.h>
#include <iostream>

class JOtherEntity {
public:
	jobject eObj;

	JOtherEntity(jobject _obj) {
		eObj = _obj;
	}

	double getX() {
		jclass c = vm.env->FindClass("pk");
		jfieldID id = vm.env->GetFieldID(c, "s", "D");
		return vm.env->GetDoubleField(eObj, id);
	};
	double getY() {
		jclass c = vm.env->FindClass("pk");
		jfieldID id = vm.env->GetFieldID(c, "t", "D");
		return vm.env->GetDoubleField(eObj, id);
	};
	double getZ() {
		jclass c = vm.env->FindClass("pk");
		jfieldID id = vm.env->GetFieldID(c, "u", "D");
		return vm.env->GetDoubleField(eObj, id);
	};

	float getHealth() {
		if (vm.env->IsInstanceOf(eObj, vm.env->FindClass("pr"))) {
			jclass c = vm.env->FindClass("pr");
			jmethodID id = vm.env->GetMethodID(c, "bn", "()F");
			return vm.env->CallFloatMethod(eObj, id);
		}
		else {
			return 0;
		}
	};

	const char* entityName() {
		jclass c = vm.env->FindClass("pk");
		jmethodID id = vm.env->GetMethodID(c, "e_", "()Ljava/lang/String;");
		jobject obj = vm.env->CallObjectMethod(eObj, id);
		return vm.env->GetStringUTFChars((jstring)obj, nullptr);
	};

	bool isValid() {
		jclass c = vm.env->FindClass("pk");

		jmethodID isAliveId = vm.env->GetMethodID(c, "ai", "()Z");
		jmethodID isInvisibleId = vm.env->GetMethodID(c, "ax", "()Z");

		return !vm.env->CallBooleanMethod(eObj, isInvisibleId) && vm.env->CallBooleanMethod(eObj, isAliveId);
	}

	AxisAlignedBB* getBoundingBox() {
		jclass c = vm.env->FindClass("pk");

		jfieldID bbId = vm.env->GetFieldID(c, "f", "Laug;");
		jobject bbObj = vm.env->GetObjectField(eObj, bbId);

		return AxisAlignedBB::fromJNI(bbObj);
	}
};