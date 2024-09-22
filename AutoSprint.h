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

class AutoSprint : public Module {
public:
    AutoSprint() : Module("AutoSprint", "To Sprint Automatically") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;
};

void AutoSprint::Enable() {
    this->isEnabled = true;
}

void AutoSprint::Disable() {
    this->isEnabled = false;
}

void AutoSprint::onTick() {
    if (this->isEnabled) {
        MinecraftInstance::getInstance()->getPlayer()->setSprinting(true);
    }
}

void AutoSprint::onRender() {

}