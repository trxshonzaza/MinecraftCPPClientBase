#pragma once

#include "Module.h"
#include "PlayerUtils.h"
#include "MinecraftInstance.h"

#include <iostream>
#include <cstdlib> 
#include <ctime>   
#include <vector>

class Aimbot : public Module {
public:
    Aimbot() : Module("Aimbot", "Automatically Aims at nearby living entities") {}

    void Enable() override;
    void Disable() override;
    void onTick() override;
    void onRender() override;
};

void Aimbot::Enable() {
    this->isEnabled = true;
}

void Aimbot::Disable() {
    this->isEnabled = false;
}

void Aimbot::onTick() {
    try {
        if (this->isEnabled) {
            std::vector<JOtherEntity*> entities = MinecraftInstance::getInstance()->getWorld()->getEntities();
            const float MAX_DISTANCE = 5.0f;
            const float HEAD_HEIGHT_OFFSET = 1.0f;

            for (JOtherEntity* entity : entities) {
                if (entity && entity->isValid()) {
                    if (PlayerUtils::isLiving(entity)) {
                        if (!PlayerUtils::sameEntity(entity) && !PlayerUtils::isItem(entity)) {
                            if (PlayerUtils::distance(entity->getX(), entity->getY(), entity->getZ()) < MAX_DISTANCE) {
                                std::vector<float> look = PlayerUtils::getLook(entity->getX(), entity->getY() + HEAD_HEIGHT_OFFSET, entity->getZ());

                                int min = -3;
                                int max = 3;
                                int r1 = min + std::rand() % (max - min + 1);
                                int r2 = min + std::rand() % (max - min + 1);

                                MinecraftInstance::getInstance()->getPlayer()->setYaw(look[0] + r1);
                                MinecraftInstance::getInstance()->getPlayer()->setPitch(look[1] + r2);
                                Sleep(20);
                            }
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in Aimbot::onTick: " << e.what() << std::endl;
    }
}

void Aimbot::onRender() {
    
}