#pragma once

#include "Module.h"
#include "PlayerUtils.h"
#include "MinecraftInstance.h"
#include "ActiveRenderInfo.h"
#include "Debug.h"
#include "JPlayer.h"

#include <iostream>
#include <vector>
#include <variant>
#include <math.h>
#include <Windows.h>

class Strafe : public Module {
public:
    Strafe() : Module("JumpStrafe", "Move faster and Automatically Jump") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;
};

void Strafe::Enable() {
    this->isEnabled = true;
}

void Strafe::Disable() {
    this->isEnabled = false;
}

void Strafe::onTick() {
    if (this->isEnabled) {
        if (MinecraftInstance::getInstance()->isPressingMovementKeys()) {
            JPlayer* player = MinecraftInstance::getInstance()->getPlayer();

            float yaw = player->getYaw();
            if (player->getMoveForward() < 0.0f) {
                yaw += 180.0f;
            }
            float forward = 1.0f;
            if (player->getMoveForward() < 0.0f) {
                forward = -0.5f;
            }
            else if (player->getMoveForward() > 0.0f) {
                forward = 0.5f;
            }
            if (player->getMoveStrafing() > 0.0f) {
                yaw -= 90.0f * forward;
            }
            if (player->getMoveStrafing() < 0.0f) {
                yaw += 90.0f * forward;
            }

            yaw *= 0.017453292f;

            Velocity* velocity = player->getVelocity();

            velocity->xVel = -sin(yaw) * 0.26F;
            velocity->zVel = cos(yaw) * 0.26F;

            player->setVelocity(velocity);

            if (player->isGround()) {
                player->jump();
            }
        }
    }
}

void Strafe::onRender() {

}