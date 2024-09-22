#pragma once

#include <jni.h>

#include "VM.h"
#include "AxisAlignedBB.h"
#include "Debug.h"

class Velocity {
public:
	double xVel, yVel, zVel;

	Velocity(double x, double y, double z) {
		xVel = x;
		yVel = y;
		zVel = z;
	};
};

class JPlayer {
public:
	jobject pObj;

	JPlayer(jobject obj) {
		pObj = obj;
	};

	double getX() {
		jclass c = vm.env->GetObjectClass(pObj);
		jfieldID id = vm.env->GetFieldID(c, "bK", "D");
		return vm.env->GetDoubleField(pObj, id);
	};
	double getY() {
		jclass c = vm.env->GetObjectClass(pObj);
		jfieldID id = vm.env->GetFieldID(c, "bL", "D");
		return vm.env->GetDoubleField(pObj, id);
	};
	double getZ() {
		jclass c = vm.env->GetObjectClass(pObj);
		jfieldID id = vm.env->GetFieldID(c, "bM", "D");
		return vm.env->GetDoubleField(pObj, id);
	};

	float getPitch() {
		jclass c = vm.env->GetObjectClass(pObj);
		jfieldID id = vm.env->GetFieldID(c, "z", "F");
		return vm.env->GetFloatField(pObj, id);
	};
	float getYaw() {
		jclass c = vm.env->GetObjectClass(pObj);
		jfieldID id = vm.env->GetFieldID(c, "y", "F");
		return vm.env->GetFloatField(pObj, id);
	};

	void setPitch(float pitch) {
		jclass c = vm.env->GetObjectClass(pObj);
		jfieldID id = vm.env->GetFieldID(c, "z", "F");
		vm.env->SetFloatField(pObj, id, pitch);
	};
	void setYaw(float yaw) {
		jclass c = vm.env->GetObjectClass(pObj);
		jfieldID id = vm.env->GetFieldID(c, "y", "F");
		vm.env->SetFloatField(pObj, id, yaw);
	};

	Velocity* getVelocity() {
		jclass c = vm.env->GetObjectClass(pObj);

		jfieldID xid = vm.env->GetFieldID(c, "v", "D");
		jfieldID yid = vm.env->GetFieldID(c, "w", "D");
		jfieldID zid = vm.env->GetFieldID(c, "x", "D");

		return new Velocity(vm.env->GetDoubleField(pObj, xid), vm.env->GetDoubleField(pObj, yid), vm.env->GetDoubleField(pObj, zid));
	};

	void setVelocity(Velocity* velocity) {
		jclass c = vm.env->GetObjectClass(pObj);

		jfieldID xid = vm.env->GetFieldID(c, "v", "D");
		jfieldID yid = vm.env->GetFieldID(c, "w", "D");
		jfieldID zid = vm.env->GetFieldID(c, "x", "D");

		vm.env->SetDoubleField(pObj, xid, velocity->xVel);
		vm.env->SetDoubleField(pObj, yid, velocity->yVel);
		vm.env->SetDoubleField(pObj, zid, velocity->zVel);
	};

	void jump() {
		jclass c = vm.env->FindClass("pr");

		jmethodID id = vm.env->GetMethodID(c, "bF", "()V");

		vm.env->CallVoidMethod(pObj, id);
	};

	bool isGround() {
		jclass c = vm.env->FindClass("pk");

		jfieldID id = vm.env->GetFieldID(c, "C", "Z");

		return vm.env->GetBooleanField(pObj, id);
	};

	jobject getHeldItem() {
		jclass c = vm.env->FindClass("wn");

		jmethodID id = vm.env->GetMethodID(c, "bZ", "()Lzx;");

		return vm.env->CallObjectMethod(pObj, id);
	};

	float getMoveForward() {
		jclass c = vm.env->FindClass("pr");

		jfieldID id = vm.env->GetFieldID(c, "ba", "F");

		return vm.env->GetFloatField(pObj, id);
	};

	float getMoveStrafing() {
		jclass c = vm.env->FindClass("pr");

		jfieldID id = vm.env->GetFieldID(c, "aZ", "F");

		return vm.env->GetFloatField(pObj, id);
	};

	void displayMessage(const char* message) {
		jmethodID addChatMessage = vm.env->GetMethodID(vm.env->FindClass("bew"), "a", "(Leu;)V");

		jclass chatComponentClass = vm.env->FindClass("fa");
		jmethodID chatComponentConstructor = vm.env->GetMethodID(chatComponentClass, "<init>", "(Ljava/lang/String;)V");

		jobject chatComponent = vm.env->NewObject(chatComponentClass, chatComponentConstructor, vm.env->NewStringUTF(message));

		vm.env->CallVoidMethod(pObj, addChatMessage, chatComponent);
	};

	bool isSneaking() {
		jclass c = vm.env->GetObjectClass(pObj);

		jmethodID id = vm.env->GetMethodID(c, "av", "()Z");

		return vm.env->CallBooleanMethod(pObj, id);
	};

	void setSneaking(bool b) {
		jclass c = vm.env->FindClass("pk");

		jmethodID id = vm.env->GetMethodID(c, "c", "(Z)V");

		return vm.env->CallVoidMethod(pObj, id, b);
	};

	void setSprinting(bool b) {
		jclass c = vm.env->GetObjectClass(pObj);

		jmethodID id = vm.env->GetMethodID(c, "d", "(Z)V");

		vm.env->CallVoidMethod(pObj, id, b);
	};

	const char* getName() {
		jclass c = vm.env->FindClass("pk");
		jmethodID id = vm.env->GetMethodID(c, "e_", "()Ljava/lang/String");
		jobject obj = vm.env->CallObjectMethod(pObj, id);
		return vm.env->GetStringUTFChars((jstring)obj, nullptr);
	};

	float getEyeHeight() {
		jclass c = vm.env->FindClass("pk");
		jmethodID id = vm.env->GetMethodID(c, "aS", "()F");
		jfloat height = vm.env->CallFloatMethod(pObj, id);
		return height;
	};

	float getFallDistance() {
		jclass c = vm.env->FindClass("pk");

		jfieldID id = vm.env->GetFieldID(c, "O", "F");

		return vm.env->GetFloatField(pObj, id);
	}

	void swing() {
		jclass c = vm.env->FindClass("pr");
		jmethodID id = vm.env->GetMethodID(c, "bw", "()V");
		vm.env->CallVoidMethod(pObj, id);
	};

	void sendPacket(jobject packet) {
		jclass c = vm.env->FindClass("bew");
		jfieldID sendQueueId = vm.env->GetFieldID(c, "a", "Lbcy;");

		jobject sendQueue = vm.env->GetObjectField(pObj, sendQueueId);

		jclass netHandlerClass = vm.env->FindClass("bcy");

		jmethodID addToSendQueueId = vm.env->GetMethodID(netHandlerClass, "a", "(Lff;)V");

		vm.env->CallVoidMethod(sendQueue, addToSendQueueId, packet);
	}

	AxisAlignedBB* getBoundingBox() {
		jclass c = vm.env->FindClass("pk");

		jfieldID bbId = vm.env->GetFieldID(c, "f", "Laug;");
		jobject bbObj = vm.env->GetObjectField(pObj, bbId);

		return AxisAlignedBB::fromJNI(bbObj);
	}
};

