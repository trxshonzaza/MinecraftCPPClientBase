#pragma once

#include "VM.h"
#include <jni.h>

class BlockPos {
public:
	double x;
	double y;
	double z;

	BlockPos(double _x, double _y, double _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	BlockPos* add(double _x, double _y, double _z) {
		return new BlockPos(x + _x, y + _y, z + _z);
	}

	jobject toJNI() {
		jclass blockPosClass = vm.env->FindClass("cj");

		jmethodID initId = vm.env->GetMethodID(blockPosClass, "<init>", "(DDD)V");

		return vm.env->NewObject(blockPosClass, initId, x, y, z);
	}
};