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

class ExampleModule : public Module {
public:
    ExampleModule() : Module("Example", "Example module to copy/paste") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;

    bool visualEnable = false;
};

void ExampleModule::Enable() {
    this->isEnabled = true;
    //visualEnable = true;
}

void ExampleModule::Disable() {
    this->isEnabled = false;
    //visualEnable = false;
}

void ExampleModule::onTick() {
    if (this->isEnabled) {
        
    }
}

void ExampleModule::onRender() {
    if (visualEnable) {
        
    }
}