#pragma once

#include <jni.h>
#include "JOtherEntity.h"
#include "JPlayer.h"
#include "MinecraftInstance.h"
#include "Module.h"
#include "EnumFacing.h"
#include "ActiveRenderInfo.h"

#include <string.h>
#include <GL/gl.h>
#include <Windows.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "Vectors.h"

class PlayerUtils {
public:
	static jobject JPlayerAsEntity();
	static bool sameEntity(JOtherEntity* otherEntity);
	static bool isItem(JOtherEntity* otherEntity);
	static bool isLiving(JOtherEntity* otherEntity);
	static std::vector<float> getLook(double posX, double posY, double posZ);
	static double distance(double posX, double posY, double posZ);
	static void getRenderManagerPositions();
	static float* WorldToScreen(Vec4 pointInWorld, GLfloat modelView[16], GLfloat projection[16]);
	static void getDimensions();
	static float* getPitchAndYawRenderVM();
	static Vec4 Multiply(Vec4 vec, GLfloat mat[16]);

	static inline double renderPosX;
	static inline double renderPosY;
	static inline double renderPosZ;

	static inline int height;
	static inline int width;

	static inline float yaw;
	static inline float pitch;
};

jobject PlayerUtils::JPlayerAsEntity() {
	jclass mcClass = vm.env->FindClass("ave");

	jmethodID getMcId = vm.env->GetStaticMethodID(mcClass, "A", "()Lave;");
	jobject minecraft = vm.env->CallStaticObjectMethod(mcClass, getMcId);

	jfieldID id = vm.env->GetFieldID(mcClass, "h", "Lbew;");

	return vm.env->GetObjectField(minecraft, id);
}

float* PlayerUtils::getPitchAndYawRenderVM() {
	jclass mcClass = vm.renderEnv->FindClass("ave");

	jmethodID getMcId = vm.renderEnv->GetStaticMethodID(mcClass, "A", "()Lave;");
	jobject minecraft = vm.renderEnv->CallStaticObjectMethod(mcClass, getMcId);

	jfieldID id = vm.renderEnv->GetFieldID(mcClass, "h", "Lbew;");

	jobject player = vm.renderEnv->GetObjectField(minecraft, id);

	jclass c = vm.renderEnv->GetObjectClass(player);

	jfieldID id1 = vm.renderEnv->GetFieldID(c, "z", "F");
	float pitch = vm.renderEnv->GetFloatField(player, id1);

	jfieldID id2 = vm.renderEnv->GetFieldID(c, "y", "F");
	float yaw = vm.renderEnv->GetFloatField(player, id2);

	return new float[2] {pitch, yaw};
}

bool PlayerUtils::sameEntity(JOtherEntity* otherEntity) {
	jclass mcClass = vm.env->FindClass("ave");

	jmethodID getMcId = vm.env->GetStaticMethodID(mcClass, "A", "()Lave;");
	jobject minecraft = vm.env->CallStaticObjectMethod(mcClass, getMcId);

	jfieldID id = vm.env->GetFieldID(mcClass, "h", "Lbew;");
	jobject p = vm.env->GetObjectField(minecraft, id);

	if (!p) {
		Debug::write("player is NULL!");
		return false;
	}

	return vm.env->IsSameObject(p, otherEntity->eObj);
}

bool PlayerUtils::isItem(JOtherEntity* otherEntity) {
	return vm.env->IsInstanceOf(otherEntity->eObj, vm.env->FindClass("uz"));
}

bool PlayerUtils::isLiving(JOtherEntity* otherEntity) {
	return vm.env->IsInstanceOf(otherEntity->eObj, vm.env->FindClass("pr"));
}

std::vector<float> PlayerUtils::getLook(double posX, double posY, double posZ) {
	JPlayer* player = new JPlayer(JPlayerAsEntity());
	double x = posX - player->getX();
	double y = posY - (player->getY() + (double)player->getEyeHeight());
	double z = posZ - player->getZ();
	double dist = (double)sqrt(x * x + z * z);
	float yaw = (float)(atan2(z, x) * 180.0 / 3.141592653589793) - 90.0F;
	float pitch = (float)(-(atan2(y, dist) * 180.0 / 3.141592653589793));
	std::vector<float> toReturn = { yaw, pitch };
	return toReturn;
}

double PlayerUtils::distance(double posX, double posY, double posZ) {
	JPlayer* player = new JPlayer(JPlayerAsEntity());
	double x = posX - player->getX();
	double y = posY - (player->getY() + (double)player->getEyeHeight());
	double z = posZ - player->getZ();
	double dist = (double)sqrt(x * x + y * y + z * z);
	return dist;
}

// Credit to BlackTeal for World2Screen methods! https://www.unknowncheats.me/forum/members/2422006.html

Vec4 PlayerUtils::Multiply(Vec4 vec, GLfloat mat[16])
{
	return Vec4(
		vec.x * mat[0] + vec.y * mat[4] + vec.z * mat[8] + vec.w * mat[12],
		vec.x * mat[1] + vec.y * mat[5] + vec.z * mat[9] + vec.w * mat[13],
		vec.x * mat[2] + vec.y * mat[6] + vec.z * mat[10] + vec.w * mat[14],
		vec.x * mat[3] + vec.y * mat[7] + vec.z * mat[11] + vec.w * mat[15]
	);
}

float* PlayerUtils::WorldToScreen(Vec4 pointInWorld, GLfloat modelView[16], GLfloat projection[16])
{
	ActiveRenderInfo::Get_VIEWPORT();

	Vec4 clipSpacePos = Multiply(Multiply(Vec4(pointInWorld.x, pointInWorld.y, pointInWorld.z, 1.0f), modelView), projection);

	Vec3 ndcSpacePos = Vec3(clipSpacePos.x / clipSpacePos.w, clipSpacePos.y / clipSpacePos.w, clipSpacePos.z / clipSpacePos.w);

	// nPlane = -1, fPlane = 1
	if (ndcSpacePos.z < -1.0 || ndcSpacePos.z > 1.0)
	{
		return new float[2] {0, 0};
	}
	float x = ((ndcSpacePos.x + 1.0f) / 2.0f) * ActiveRenderInfo::VIEWPORT[2];
	float y = ((1.0f - ndcSpacePos.y) / 2.0f) * ActiveRenderInfo::VIEWPORT[3];

	std::cout << x << ", " << y << std::endl;
	return new float[2] {x, y};
}

void PlayerUtils::getRenderManagerPositions() {
	jmethodID renderManagerId = vm.renderEnv->GetMethodID(vm.renderEnv->FindClass("ave"), "af", "()Lbiu;");

	jmethodID id = vm.renderEnv->GetStaticMethodID(vm.renderEnv->FindClass("ave"), "A", "()Lave;"); //Minecraft instance
	jobject mc = vm.renderEnv->CallStaticObjectMethod(vm.renderEnv->FindClass("ave"), id);

	if (renderManagerId == NULL) {
		Debug::write("renderManager method is NULL!");
		return;
	}

	jobject renderManager = vm.renderEnv->CallObjectMethod(mc, renderManagerId);

	if (renderManager == NULL) {
		Debug::write("renderManager object is NULL!");
		return;
	}

	jclass renderManagerClass = vm.renderEnv->FindClass("biu");

	jfieldID xid = vm.renderEnv->GetFieldID(renderManagerClass, "o", "D");
	jfieldID yid = vm.renderEnv->GetFieldID(renderManagerClass, "p", "D");
	jfieldID zid = vm.renderEnv->GetFieldID(renderManagerClass, "q", "D");

	renderPosX = vm.renderEnv->GetDoubleField(renderManager, xid);
	renderPosY = vm.renderEnv->GetDoubleField(renderManager, yid);
	renderPosZ = vm.renderEnv->GetDoubleField(renderManager, zid);
}

void PlayerUtils::getDimensions() {
	jclass displayClass = vm.renderEnv->FindClass("org/lwjgl/opengl/Display");

	jmethodID heightId = vm.renderEnv->GetStaticMethodID(displayClass, "getHeight", "()I");
	jmethodID widthId = vm.renderEnv->GetStaticMethodID(displayClass, "getWidth", "()I");

	height = vm.renderEnv->CallStaticIntMethod(displayClass, heightId);
	width = vm.renderEnv->CallIntMethod(displayClass, widthId);
}