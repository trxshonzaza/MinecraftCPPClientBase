#pragma once

#include <jni.h>
#include "AxisAlignedBB.h"
#include "VM.h"
#include "JOtherEntity.h"
#include "PlayerUtils.h"
#include "BlockPos.h"
#include "Debug.h"

#include <vector>
#include <iostream>

class JWorld {
public:
	jobject wObj;

	JWorld(jobject obj) {
		wObj = obj;
	};

	std::vector<JOtherEntity*> getEntities();
	bool setBlockState(jobject blockPos, jobject blockState);
	jobject getBlockState(jobject blockPos);
	bool isValid(BlockPos* pos);
};

std::vector<JOtherEntity*> JWorld::getEntities() {
    std::vector<JOtherEntity*> entityList = {};

	jclass worldClientClass = vm.env->GetObjectClass(wObj);
	jclass worldClass = vm.env->GetSuperclass(worldClientClass);

	jfieldID entityListId = vm.env->GetFieldID(worldClass, "f", "Ljava/util/List;");

	jobject list = vm.env->GetObjectField(wObj, entityListId);
    jclass listClass = vm.env->GetObjectClass(list);

	jmethodID sizeId = vm.env->GetMethodID(listClass, "size", "()I");	
	jint size = vm.env->CallIntMethod(list, sizeId);

	jmethodID getId = vm.env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

	for (jint i = 0; i < size; i++) {
		jobject entity = vm.env->CallObjectMethod(list, getId, i);

		entityList.push_back(new JOtherEntity(entity));
	}

	return entityList;
}

bool JWorld::setBlockState(jobject blockPos, jobject blockState) {
	jclass worldClientClass = vm.env->GetObjectClass(wObj);
	jclass worldClass = vm.env->GetSuperclass(worldClientClass);

	jmethodID setBlockStateId = vm.env->GetMethodID(worldClass, "a", "(Lcj;Lalz;)Z");

	return vm.env->CallBooleanMethod(wObj, setBlockStateId, blockPos, blockState);
}

jobject JWorld::getBlockState(jobject blockPos) {
	jclass worldClientClass = vm.env->GetObjectClass(wObj);
	jclass worldClass = vm.env->GetSuperclass(worldClientClass);

	jmethodID getBlockStateId = vm.env->GetMethodID(worldClass, "p", "(Lcj;)Lalz;");

	return vm.env->CallObjectMethod(wObj, getBlockStateId, blockPos);
}

bool JWorld::isValid(BlockPos* pos) {
	jobject blockState = getBlockState(pos->toJNI());

	jclass blockStateClass = vm.env->FindClass("alz");
	jmethodID getBlockId = vm.env->GetMethodID(blockStateClass, "c", "()Lafh;");
	jobject blockObject = vm.env->CallObjectMethod(blockState, getBlockId);

	jclass blocksClass = vm.env->FindClass("afi");
	jfieldID airId = vm.env->GetStaticFieldID(blocksClass, "a", "Lafh;");

	jobject airObject = vm.env->GetStaticObjectField(blocksClass, airId);
	jobject liquidObject = vm.env->GetStaticObjectField(blocksClass, airId);

	if (!vm.env->IsSameObject(blockObject, airObject) && !vm.env->IsInstanceOf(blockObject, vm.env->FindClass("ahv"))) {
		return true;
	}

	return false;
}