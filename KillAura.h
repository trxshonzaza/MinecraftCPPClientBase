#pragma once

#include "Module.h"
#include "MinecraftInstance.h"
#include "PlayerUtils.h"
#include "JOtherEntity.h"
#include "JPlayer.h"
#include "VM.h"

#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <vector>

class KillAura : public Module {
public:
    KillAura() : Module("KillAura", "Attacks Nearby Entities") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;

    static inline JOtherEntity* targeted;
};

void KillAura::Enable() {
    this->isEnabled = true;
}

void KillAura::Disable() {
    this->isEnabled = false;
    targeted == NULL;
}

void KillAura::onTick() {
    if (this->isEnabled) {
        std::vector<JOtherEntity*> toProcess = MinecraftInstance::getInstance()->getWorld()->getEntities();
        JPlayer* player = MinecraftInstance::getInstance()->getPlayer();

        for (JOtherEntity* entity : toProcess) {
            if (entity) {
                if (entity->isValid()) {
                    if (PlayerUtils::isLiving(entity)) {
                        if (!PlayerUtils::sameEntity(entity)) {
                            if (PlayerUtils::distance(entity->getX(), entity->getY(), entity->getZ()) <= 4.35F) {
                                if (targeted == NULL) {
                                    targeted = entity;
                                    MinecraftInstance::getInstance()->attackEntity(entity);
                                    player->swing();
                                    int min = 50;
                                    int max = 200;
                                    int r1 = min + std::rand() % (max - min + 1);
                                    Sleep(r1);
                                    break;
                                }
                                else {
                                    if (vm.env->IsSameObject(targeted->eObj, entity->eObj)) {
                                        targeted = entity;
                                        MinecraftInstance::getInstance()->attackEntity(entity);
                                        player->swing();
                                        int min = 50;
                                        int max = 200;
                                        int r1 = min + std::rand() % (max - min + 1);
                                        Sleep(r1);
                                        break;
                                    }
                                }
                            }
                            else if (targeted != NULL) {
                                targeted = NULL;
                            }
                        }
                    }
                }
            }
        }
    }
}

void KillAura::onRender() {

}