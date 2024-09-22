#pragma once

#pragma once

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

class ESP : public Module {
public:
    ESP() : Module("ESP", "Draws Boxes at nearby entites") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;
};

struct EspEntityData {
    AxisAlignedBB* bb;
    double x;
    double y;
    double z;
    float h;
};

std::vector<EspEntityData> espEntityData = {};

/*
* THE ORDER OF THE ENTITY DATA TO BE PROCESSED IS AS FOLLOWS FROM TOP TO BOTTOM:
* AxisAlignedBB (entity bounding box)
* X value (double)
* Y value (double)
* Z value (double)
* Health value (float)
* TODO: Name Value (const char*)
*/

void ESP::Enable() {
    this->isEnabled = true;
}

void ESP::Disable() {
    this->isEnabled = false;
}

void ESP::onTick() {
    if (this->isEnabled) {
        std::vector<JOtherEntity*> toProcess = MinecraftInstance::getInstance()->getWorld()->getEntities();

        espEntityData.clear();

        for (JOtherEntity* entity : toProcess) {
            if (entity) {
                if (entity->isValid()) {
                    if (PlayerUtils::isLiving(entity)) {
                        if (!PlayerUtils::sameEntity(entity)) {
                            EspEntityData data;
                            data.bb = entity->getBoundingBox();
                            data.x = entity->getX();
                            data.y = entity->getY();
                            data.z = entity->getZ();
                            data.h = entity->getHealth();

                            espEntityData.push_back(data);
                        }
                    }
                }
            }
        }
    }
}

void ESP::onRender() {
    if (this->isEnabled) {
        glLineWidth(1.5);

        for (const auto& data : espEntityData) {
            double maxX = data.bb->maxX - PlayerUtils::renderPosX;
            double maxY = data.bb->maxY - PlayerUtils::renderPosY;
            double maxZ = data.bb->maxZ - PlayerUtils::renderPosZ;
            double minX = data.bb->minX - PlayerUtils::renderPosX;
            double minY = data.bb->minY - PlayerUtils::renderPosY;
            double minZ = data.bb->minZ - PlayerUtils::renderPosZ;

            for (int i = 0; i < 5; i++) {
                glBegin(GL_LINES);
                glColor4f(1, 0, 0, 1);

                glVertex3f(minX, minY, minZ);
                glVertex3f(maxX, minY, minZ);
                glVertex3f(minX, minY, minZ);
                glVertex3f(minX, minY, maxZ);
                glVertex3f(maxX, minY, maxZ);
                glVertex3f(minX, minY, maxZ);
                glVertex3f(maxX, minY, maxZ);
                glVertex3f(maxX, minY, minZ);

                glVertex3f(minX, minY, minZ);
                glVertex3f(minX, maxY, minZ);
                glVertex3f(maxX, minY, minZ);
                glVertex3f(maxX, maxY, minZ);
                glVertex3f(minX, minY, maxZ);
                glVertex3f(minX, maxY, maxZ);
                glVertex3f(maxX, minY, maxZ);
                glVertex3f(maxX, maxY, maxZ);

                glVertex3f(minX, maxY, minZ);
                glVertex3f(maxX, maxY, minZ);
                glVertex3f(minX, maxY, minZ);
                glVertex3f(minX, maxY, maxZ);
                glVertex3f(maxX, maxY, maxZ);
                glVertex3f(minX, maxY, maxZ);
                glVertex3f(maxX, maxY, maxZ);
                glVertex3f(maxX, maxY, minZ);
                glEnd();
            }
        }
    }
}