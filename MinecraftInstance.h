#pragma once

#include <jni.h>
#include <stdio.h>

#include <iostream>
#include <vector>

#include "JPlayer.h"
#include "Debug.h"
#include "BlockPos.h"
#include "VM.h"
#include "JWorld.h"

class MinecraftInstance {
private:
	static MinecraftInstance* instance;
public:
	static void setInstance(MinecraftInstance* _inst) {
		instance = _inst;
	}

	static MinecraftInstance* getInstance() {
		return instance;
	}

	JPlayer* getPlayer();
	JWorld* getWorld();
	jclass getMcClass();
	jobject getMinecraft();
	void attackEntity(JOtherEntity* entity);
	bool isInWorld();
	std::vector<float> getRenderManagerPlayerViewPositions();
	void placeBlock(BlockPos* pos, jobject item, EnumFacing facing, std::vector<float> lookDirs);
	bool isPressingMovementKeys();
	jobject getPlayerController();
	jobject enumFacingToJNI(EnumFacing facing);
	jobject newVec3D(double x, double y, double z);
	jobject getCurrentGui();
	void sendDirectPacket(jobject packet);
	void windowClick(int id, int index, int i1, int i2);
};

jclass MinecraftInstance::getMcClass() {
	return vm.env->FindClass("ave"); // Minecraft class
}

jobject MinecraftInstance::getMinecraft() {
	jmethodID id = vm.env->GetStaticMethodID(getMcClass(), "A", "()Lave;"); //Minecraft instance
	return vm.env->CallStaticObjectMethod(getMcClass(), id);
}

bool MinecraftInstance::isInWorld() {
	jfieldID id = vm.env->GetFieldID(getMcClass(), "f", "Lbdb;");
	jobject w = vm.env->GetObjectField(getMinecraft(), id);

	return !(!w);
}

JPlayer* MinecraftInstance::getPlayer() {
	jfieldID id = vm.env->GetFieldID(getMcClass(), "h", "Lbew;"); // EntityPlayerSP
	jobject p = vm.env->GetObjectField(getMinecraft(), id);

	if (!p) {
		Debug::write("player is NULL!");
		return nullptr;
	}

	return new JPlayer(p);
}

JWorld* MinecraftInstance::getWorld() {
	jfieldID id = vm.env->GetFieldID(getMcClass(), "f", "Lbdb;"); // World 
	jobject w = vm.env->GetObjectField(getMinecraft(), id);

	if (!w) {
		Debug::write("world is NULL!");
		return nullptr;
	}

	return new JWorld(w);
}

std::vector<float> MinecraftInstance::getRenderManagerPlayerViewPositions() {
	jfieldID renderManagerId = vm.env->GetFieldID(getMcClass(), "aa", "Lbiu;");
	jobject renderManager = vm.env->GetObjectField(getMinecraft(), renderManagerId);

	jfieldID xid = vm.env->GetFieldID(getMcClass(), "f", "F");
	jfieldID yid = vm.env->GetFieldID(getMcClass(), "e", "F");

	std::vector<float> pos = {};

	pos.push_back(vm.env->GetFloatField(renderManager, xid));
	pos.push_back(vm.env->GetFloatField(renderManager, yid));

	return pos;
}

void MinecraftInstance::attackEntity(JOtherEntity* entity) {
	jfieldID id = vm.env->GetFieldID(getMcClass(), "h", "Lbew;"); // EntityPlayerSP
	jobject p = vm.env->GetObjectField(getMinecraft(), id);

	jfieldID pControllerId = vm.env->GetFieldID(getMcClass(), "c", "Lbda;");
	jobject pController = vm.env->GetObjectField(getMinecraft(), pControllerId);

	jclass pControllerClass = vm.env->FindClass("bda");

	jmethodID attackEntityId = vm.env->GetMethodID(pControllerClass, "a", "(Lwn;Lpk;)V");

	vm.env->CallVoidMethod(pController, attackEntityId, p, entity->eObj);
}

bool MinecraftInstance::isPressingMovementKeys() {
	jfieldID gameSettingsId = vm.env->GetFieldID(getMcClass(), "t", "Lavh;");
	jobject gameSettings = vm.env->GetObjectField(getMinecraft(), gameSettingsId);

	jclass gameSettingsClass = vm.env->FindClass("avh");

	jfieldID fId = vm.env->GetFieldID(gameSettingsClass, "Y", "Lavb;");
	jfieldID bId = vm.env->GetFieldID(gameSettingsClass, "aa", "Lavb;");
	jfieldID lId = vm.env->GetFieldID(gameSettingsClass, "Z", "Lavb;");
	jfieldID rId = vm.env->GetFieldID(gameSettingsClass, "ab", "Lavb;");

	jobject fObj = vm.env->GetObjectField(gameSettings, fId);
	jobject bObj = vm.env->GetObjectField(gameSettings, bId);
	jobject lObj = vm.env->GetObjectField(gameSettings, lId);
	jobject rObj = vm.env->GetObjectField(gameSettings, rId);

	jclass keyBindingClass = vm.env->FindClass("avb");

	jfieldID pressedId = vm.env->GetFieldID(keyBindingClass, "h", "Z");

	return (vm.env->GetBooleanField(fObj, pressedId) || vm.env->GetBooleanField(bObj, pressedId) || vm.env->GetBooleanField(lObj, pressedId) || vm.env->GetBooleanField(rObj, pressedId));
}

void MinecraftInstance::placeBlock(BlockPos* pos, jobject item, EnumFacing facing, std::vector<float> lookDirs) {
	jclass blockItemClass = vm.env->FindClass("yo");

	jobject playerController = getPlayerController();
	jclass playerControllerClass = vm.env->FindClass("bda");
	JPlayer* player = getPlayer();
	JWorld* world = getWorld();
	jobject posObj = pos->toJNI();
	jobject enumFacingObj = enumFacingToJNI(facing);
	jobject vec3d = newVec3D(0.5F, 0.5F, 0.5F);

	jmethodID onRightClickId = vm.env->GetMethodID(playerControllerClass, "a", "(Lbew;Lbdb;Lzx;Lcj;Lcq;Laui;)Z");

	jboolean returned = vm.env->CallBooleanMethod(playerController, onRightClickId,
		player->pObj, world->wObj, item, posObj, enumFacingObj, vec3d);

	player->swing();

	jclass posAndlookPacketClass = vm.env->FindClass("ip$b"); 	// C03PacketPlayer.C05PacketPlayerPosLook
	jmethodID posAndlookPacketConstructor = vm.env->GetMethodID(posAndlookPacketClass, "<init>", "(DDDFFZ)V");

	double x = pos->x + 0.25 - player->getX();
	double y = pos->y + 0.25 - player->getY();
	double z = pos->z + 0.25 - player->getZ();

	float yaw = lookDirs[0];
	float pitch = lookDirs[1];
	bool onGround = player->isGround();

	jobject posAndLookPacketObject = vm.env->NewObject(posAndlookPacketClass, posAndlookPacketConstructor, x, y, z, yaw, pitch, onGround);
	//player->sendPacket(posAndLookPacketObject);
}

jobject MinecraftInstance::getPlayerController() {
	jfieldID playerControllerId = vm.env->GetFieldID(getMcClass(), "c", "Lbda;");
	return vm.env->GetObjectField(getMinecraft(), playerControllerId);
}

jobject MinecraftInstance::enumFacingToJNI(EnumFacing facing) {
	jclass enumClass = vm.env->FindClass("cq");
	jfieldID enumFieldID = nullptr;

	switch (facing) {
	case(DOWN):
		enumFieldID = vm.env->GetStaticFieldID(enumClass, "a", "Lcq;");
		break;
	case(UP):
		enumFieldID = vm.env->GetStaticFieldID(enumClass, "b", "Lcq;");
		break;
	case(NORTH):
		enumFieldID = vm.env->GetStaticFieldID(enumClass, "c", "Lcq;");
		break;
	case(SOUTH):
		enumFieldID = vm.env->GetStaticFieldID(enumClass, "d", "Lcq;");
		break;
	case(WEST):
		enumFieldID = vm.env->GetStaticFieldID(enumClass, "e", "Lcq;");
		break;
	case(EAST):
		enumFieldID = vm.env->GetStaticFieldID(enumClass, "f", "Lcq;");
		break;
	}

	return vm.env->GetStaticObjectField(enumClass, enumFieldID);
}

jobject MinecraftInstance::newVec3D(double x, double y, double z) {
	jclass vec3dClass = vm.env->FindClass("aui");

	jmethodID initId = vm.env->GetMethodID(vec3dClass, "<init>", "(DDD)V");

	return vm.env->NewObject(vec3dClass, initId, x, y, z);
}

void MinecraftInstance::sendDirectPacket(jobject packet) {
	jclass NPCclass = vm.env->FindClass("bcy");
	jclass netManagerClass = vm.env->FindClass("ek");

	jmethodID getNPCId = vm.env->GetMethodID(getMcClass(), "u", "()Lbcy;");
	jobject NPCobject = vm.env->CallObjectMethod(getMinecraft(), getNPCId);

	jfieldID netManagerId = vm.env->GetFieldID(NPCclass, "c", "Lek;");
	jobject netManagerObject = vm.env->GetObjectField(NPCobject, netManagerId);

	jmethodID sendPacketId = vm.env->GetMethodID(netManagerClass, "a", "(Lff;)V");
	
	vm.env->CallVoidMethod(netManagerObject, sendPacketId, packet);
}

jobject MinecraftInstance::getCurrentGui() {
	jfieldID currentGuiId = vm.env->GetFieldID(getMcClass(), "m", "Laxu;");
	return vm.env->GetObjectField(getMinecraft(), currentGuiId);
}

void MinecraftInstance::windowClick(int id, int index, int i1, int i2) {
	jclass playerControllerClass = vm.env->FindClass("bda");

	jobject playerController = getPlayerController();
	JPlayer* player = getPlayer();

	jmethodID windowClickId = vm.env->GetMethodID(playerControllerClass, "a", "(IIIILwn;)Lzx;");

	jobject returned = vm.env->CallObjectMethod(playerController, windowClickId,
		id, index, i1, i2, player->pObj);
}