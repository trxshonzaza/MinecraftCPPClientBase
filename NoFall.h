#pragma once

#include "Module.h"
#include "PlayerUtils.h"
#include "MinecraftInstance.h"
#include "ActiveRenderInfo.h"
#include "Debug.h"
#include "JPlayer.h"
#include "Vectors.h"

#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <variant>
#include <Windows.h>

class NoFall : public Module {
public:
    NoFall() : Module("NoFall", "Prevents player from taking any fall damage.") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;
};

void NoFall::Enable() {
    this->isEnabled = true;
}

void NoFall::Disable() {
    this->isEnabled = false;
}

void NoFall::onTick() {
    if (this->isEnabled) {
        JPlayer* player = MinecraftInstance::getInstance()->getPlayer();

        if (player->getFallDistance() > 2) {
            jclass lookPacketClass = vm.env->FindClass("ip$c");
            jmethodID lookPacketConstructor = vm.env->GetMethodID(lookPacketClass, "<init>", "(FFZ)V");

            float yaw = player->getYaw();
            float pitch = player->getPitch();

            jobject lookPacketObject = vm.env->NewObject(lookPacketClass, lookPacketConstructor, yaw, pitch, true);
            MinecraftInstance::getInstance()->sendDirectPacket(lookPacketObject);
        }
    }
}

void NoFall::onRender() {

}