#pragma once

#pragma once

#include "Module.h"
#include "PlayerUtils.h"
#include "MinecraftInstance.h"
#include "ActiveRenderInfo.h"
#include "Debug.h"
#include "KillAura.h"
#include "TextRenderer.h"
#include "JPlayer.h"
#include "RainbowUtil.h"
#include "JOtherEntity.h"

#include <iostream>
#include <vector>
#include <variant>
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>

class TargetStrafe : public Module {
public:
    TargetStrafe() : Module("TargetStrafe", "Automatically Strafe at opponent when using KillAura") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;

    bool canDraw = false;
};

struct TargetStrafeEntityData {
    AxisAlignedBB* bb;
    double x;
    double y;
    double z;
    float h;
    const char* name;
};

TargetStrafeEntityData tData;
auto startTime = std::chrono::steady_clock::now();

void TargetStrafe::Enable() {
    this->isEnabled = true;
}

void TargetStrafe::Disable() {
    this->isEnabled = false;
    TextRenderer::stopQueuedEntityData();
}

void TargetStrafe::onTick() {
    if (this->isEnabled) {
        JOtherEntity* targeted = KillAura::targeted;

        if (targeted != NULL) {
            JPlayer* player = MinecraftInstance::getInstance()->getPlayer();
            std::vector<float> direction = PlayerUtils::getLook(targeted->getX(), targeted->getY(), targeted->getZ());

            float yaw = direction[0];

            float forward = 1.0F;

            /*if (PlayerUtils::distance(targeted->getX(), targeted->getY(), targeted->getZ()) < 3.5) {
                yaw -= 90.0F * forward;
            }*/

            yaw *= 0.017453292F * forward;
            yaw -= 45.0F * forward;

            Velocity* velocity = player->getVelocity();

            velocity->xVel = -sin(yaw) * 0.35F;
            velocity->zVel = cos(yaw) * 0.35F;

            player->setVelocity(velocity);

            if (player->isGround()) {
                player->jump();
            }

            tData.x = targeted->getX();
            tData.y = targeted->getY();
            tData.z = targeted->getZ();
            tData.bb = targeted->getBoundingBox();
            tData.h = targeted->getHealth();
            tData.name = targeted->entityName();

            canDraw = true;
        }
        else {
            canDraw = false;
        }
    }
}

int increment = 0;
int change = -0.1;

float oscillate(float time, float min, float max, float speed) {
    float sineValue = 0.5f * (1 + std::sin(speed * (time / 1000.0f) * 2 * M_PI));
    return min + (max - min) * sineValue;
}

void TargetStrafe::onRender() {
    if (this->isEnabled && canDraw) {
        JOtherEntity* targeted = KillAura::targeted;

        if (targeted != NULL) {
            TextRenderer::entityDataToRenderName = tData.name;
            TextRenderer::entityDataToRenderHealth = round(tData.h);
            TextRenderer::startQueuedEntityData();

            auto now = std::chrono::steady_clock::now();
            auto elapsedMillis = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

            float a = oscillate(elapsedMillis, tData.bb->minY, tData.bb->maxY, 0.5F);

            double x = tData.x - PlayerUtils::renderPosX;
            double y = a - PlayerUtils::renderPosY;
            double z = tData.z - PlayerUtils::renderPosZ;

            glLineWidth(4.0f);

            glBegin(GL_LINE_LOOP);

            float p = M_PI * 2.0;
            
            for (int draw = 0; draw < 5; draw++) {
                for (int i = 0; i <= 90; ++i) {
                    float r, g, b;
                    RainbowUtil::get(i, r, g, b, 2.0F);
                    glColor4f(r, g, b, 1);
                    glVertex3d(x + 0.7F * cos(i * p / 45.0), y, z + 0.7F * sin(i * p / 45.0));
                }
            }

            glEnd();
        }
        else {
            //TextRenderer::stopQueuedEntityData();
        }
    }
}