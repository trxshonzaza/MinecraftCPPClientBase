#pragma once


#include "Module.h"
#include "PlayerUtils.h"
#include "MinecraftInstance.h"
#include "ActiveRenderInfo.h"
#include "Debug.h"
#include "Vectors.h"

#include <iostream>
#include <vector>
#include <variant>
#include <Windows.h>

class ChestStealer : public Module {
public:
    ChestStealer() : Module("ChestStealer", "Automatically takes items from a chest.") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;
};

bool isSearching = false;
bool hasItems = false;

void ChestStealer::Enable() {
    this->isEnabled = true;
}

void ChestStealer::Disable() {
    this->isEnabled = false;
}

void ChestStealer::onTick() {
    if (this->isEnabled) {
        jobject currentGui = MinecraftInstance::getInstance()->getCurrentGui();
        jclass guiChestClass = vm.env->FindClass("ayr");
        jclass inventoryClass = vm.env->FindClass("og");

        if (currentGui != nullptr && !isSearching) {
            if (vm.env->IsInstanceOf(currentGui, guiChestClass)) {
                jfieldID inventoryRowsId = vm.env->GetFieldID(guiChestClass, "x", "I");
                int rows = vm.env->GetIntField(currentGui, inventoryRowsId);

                for (int i = 0; i < rows * 9; i++) {
                    jfieldID getSlotsId = vm.env->GetFieldID(guiChestClass, "h", "Lxi;");
                    jobject containerObject = vm.env->GetObjectField(currentGui, getSlotsId);

                    jfieldID windowField = vm.env->GetFieldID(vm.env->GetObjectClass(containerObject), "d", "I");
                    int windowId = vm.env->GetIntField(containerObject, windowField);

                    jclass containerClass = vm.env->FindClass("xi");

                    jfieldID slotListId = vm.env->GetFieldID(containerClass, "c", "Ljava/util/List;");

                    jobject list = vm.env->GetObjectField(containerObject, slotListId);
                    jclass listClass = vm.env->GetObjectClass(list);

                    jmethodID getId = vm.env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

                    jobject slot = vm.env->CallObjectMethod(list, getId, i);

                    jclass slotClass = vm.env->GetObjectClass(slot);

                    jmethodID getStackId = vm.env->GetMethodID(slotClass, "d", "()Lzx;");

                    jobject stack = vm.env->CallObjectMethod(slot, getStackId);

                    if (stack != NULL) {
                        MinecraftInstance::getInstance()->windowClick(windowId, i, 0, 1);
                        Debug::write("taking item.");
                        Sleep(70);
                    }
                }

                /*jfieldID lowerInventoryId = vm.env->GetFieldID(guiChestClass, "w", "Log;");
                jobject lowerInventoryObj = vm.env->GetObjectField(currentGui, lowerInventoryId);

                jmethodID getInventorySizeId = vm.env->GetMethodID(inventoryClass, "o_", "()I");
                int inventorySize = vm.env->CallIntMethod(lowerInventoryObj, getInventorySizeId);

                jfieldID getSlotsId = vm.env->GetFieldID(guiChestClass, "h", "Lxi;");
                jobject containerObject = vm.env->GetObjectField(currentGui, getSlotsId);

                jfieldID windowField = vm.env->GetFieldID(vm.env->GetObjectClass(containerObject), "d", "I");
                int windowId = vm.env->GetIntField(containerObject, windowField);

                isSearching = true;

                for (int i = 0; i < inventorySize; ++i) {
                    jmethodID getStackId = vm.env->GetMethodID(inventoryClass, "a", "(I)Lzx;");
                    jobject stack = vm.env->CallObjectMethod(lowerInventoryObj, getStackId);

                    std::cout << stack << std::endl;

                    if (stack == NULL) {
                        Debug::write("stack is null.");
                        continue;
                    }
                    else {
                        Debug::write("stack isnt null..");
                    }

                    hasItems = true;
                    break;
                }

                if (hasItems) {
                    for (int i = 0; i < inventorySize; ++i) {
                        jmethodID getStackId = vm.env->GetMethodID(inventoryClass, "a", "(I)Lzx;");
                        jobject stack = vm.env->CallObjectMethod(lowerInventoryObj, getStackId);

                        if (stack == NULL) {
                            Debug::write("stack is null.");
                            continue;
                        }
                        else {
                            Debug::write("stack isnt null..");
                        }

                        std::cout << stack << std::endl;

                        Debug::write("taking item.");
                        MinecraftInstance::getInstance()->windowClick(windowId, i, 0, 1);
                        Sleep(30);
                    }
                }
                else {
                    Debug::write("there are no items in the chest.");
                }

                isSearching = false;
                hasItems = false;*/
            }
        }
    }
}

void ChestStealer::onRender() {

}