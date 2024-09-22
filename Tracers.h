#pragma once

#include "Module.h"
#include "PlayerUtils.h"
#include "MinecraftInstance.h"
#include "ActiveRenderInfo.h"
#include "Debug.h"
#include "Vectors.h"

#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <variant>
#include <Windows.h>

class Tracers : public Module {
public:
    Tracers() : Module("Tracers", "Draws Lines at nearby living entities") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;
};

struct TracersEntityData {
    AxisAlignedBB* bb;
    double x;
    double y;
    double z;
    float h;
};

std::vector<TracersEntityData> tracersEntityData = {};

/*
* THE ORDER OF THE ENTITY DATA TO BE PROCESSED IS AS FOLLOWS FROM TOP TO BOTTOM:
* AxisAlignedBB (entity bounding box)
* X value (double)
* Y value (double)
* Z value (double)
* Health value (float)
* TODO: Name Value (const char*)
*/

void Tracers::Enable() {
    this->isEnabled = true;
}

void Tracers::Disable() {
    this->isEnabled = false;
}

void Tracers::onTick() {    
    if (this->isEnabled) {
        std::vector<JOtherEntity*> toProcess = MinecraftInstance::getInstance()->getWorld()->getEntities();

        tracersEntityData.clear();

        for (JOtherEntity* entity : toProcess) {
            if (entity) {
                if (entity->isValid()) {
                    if (PlayerUtils::isLiving(entity)) {
                        if (!PlayerUtils::sameEntity(entity)) {
                            TracersEntityData data;
                            data.bb = entity->getBoundingBox();
                            data.x = entity->getX();
                            data.y = entity->getY();
                            data.z = entity->getZ();
                            data.h = entity->getHealth();

                            tracersEntityData.push_back(data);
                        }
                    }
                }
            }
        }
    }
}

void Tracers::onRender() {
    if (this->isEnabled) {
        glLineWidth(1.5);

        for (const auto& data : tracersEntityData) {
            glColor4f(1, 0, 0, 1);

            double x = data.x - PlayerUtils::renderPosX;
            double y = data.y - PlayerUtils::renderPosY;
            double z = data.z - PlayerUtils::renderPosZ;

            glBegin(GL_LINE_LOOP);
            glVertex3f(0, 0, 0);
            glVertex3f(x, y, z);
            glEnd();
        }
    }
}