#pragma once

#include <jni.h>
#include <iostream>
#include "VM.h"

class AxisAlignedBB {
public:
	double minX, minY, minZ, maxX, maxY, maxZ;

	AxisAlignedBB(double _minX, double _minY, double _minZ, double _maxX, double _maxY, double _maxZ) {
		minX = _minX;
		minY = _minY;
		minZ = _minZ;
		maxX = _maxX;
		maxY = _maxY;
		maxZ = _maxZ;
	};

	jobject toJNI() {
		jclass bbClass = vm.env->FindClass("aug");

		jmethodID init = vm.env->GetMethodID(bbClass, "<init>", "(DDDDDD)V");

		return vm.env->NewObject(bbClass, init, minX, minY, minZ, maxX, maxY, maxZ);
	};

	static AxisAlignedBB* fromJNI(jobject obj) {
		jclass bbClass = vm.env->FindClass("aug");

		jfieldID minXid = vm.env->GetFieldID(bbClass, "a", "D");
		jfieldID minYid = vm.env->GetFieldID(bbClass, "b", "D");
		jfieldID minZid = vm.env->GetFieldID(bbClass, "c", "D");
		jfieldID maxXid = vm.env->GetFieldID(bbClass, "d", "D");
		jfieldID maxYid = vm.env->GetFieldID(bbClass, "e", "D");
		jfieldID maxZid = vm.env->GetFieldID(bbClass, "f", "D");

		return new AxisAlignedBB(
			vm.env->GetDoubleField(obj, minXid),
			vm.env->GetDoubleField(obj, minYid),
			vm.env->GetDoubleField(obj, minZid),
			vm.env->GetDoubleField(obj, maxXid),
			vm.env->GetDoubleField(obj, maxYid),
			vm.env->GetDoubleField(obj, maxZid)
		);
	}
};