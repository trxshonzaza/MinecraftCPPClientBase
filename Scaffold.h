#pragma once

#include "Module.h"
#include "PlayerUtils.h"
#include "MinecraftInstance.h"
#include "ActiveRenderInfo.h"
#include "Debug.h"
#include "Vectors.h"
#include "EnumFacing.h"

#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <variant>
#include <Windows.h>

class Scaffold : public Module {
public:
    Scaffold() : Module("ScaffoldWalk", "Automatically Places Blocks When Walking over air.") {}

    void Enable() override;
    void Disable() override;
    void onRender() override;
    void onTick() override;
};

void Scaffold::Enable() {
    this->isEnabled = true;
}

void Scaffold::Disable() {
    this->isEnabled = false;
}

void place(BlockPos* pos, EnumFacing facing) {
	if (facing == EnumFacing::UP)
		pos = pos->add(0, -1, 0);
	else if (facing == EnumFacing::NORTH)
		pos = pos->add(0, 0, 1);
	else if (facing == EnumFacing::EAST)
		pos = pos->add(-1, 0, 0);
	else if (facing == EnumFacing::SOUTH)
		pos = pos->add(0, 0, -1);
	else if (facing == EnumFacing::WEST)
		pos = pos->add(1, 0, 0);

    JPlayer* player = MinecraftInstance::getInstance()->getPlayer();
    jobject itemStack = player->getHeldItem();

    std::vector<float> lookDirs = PlayerUtils::getLook(pos->x, pos->y, pos->z);

    MinecraftInstance::getInstance()->placeBlock(pos, itemStack, facing, lookDirs);
}

void Scaffold::onTick() {
    if (this->isEnabled) {
        JPlayer* player = MinecraftInstance::getInstance()->getPlayer();

        double x = player->getX();
        double y = player->getBoundingBox()->minY;
        double z = player->getZ();

        jobject itemStack = player->getHeldItem();

        BlockPos* pos = new BlockPos(x, y, z);

        jobject blockState = MinecraftInstance::getInstance()->getWorld()->getBlockState(pos->toJNI());

        jclass blockStateClass = vm.env->FindClass("alz");

        jmethodID getBlockId = vm.env->GetMethodID(blockStateClass, "c", "()Lafh;");

        jobject blockObject = vm.env->CallObjectMethod(blockState, getBlockId);

        if (itemStack != NULL) {
            jmethodID itemStackAsItemId = vm.env->GetMethodID(vm.env->GetObjectClass(itemStack), "b", "()Lzw;");
            jobject item = vm.env->CallObjectMethod(itemStack, itemStackAsItemId);

            JWorld* world = MinecraftInstance::getInstance()->getWorld();

            // credit to https://github.com/Gamrdude4life for scaffold algoryhthm.

            if (vm.env->IsInstanceOf(item, vm.env->FindClass("yo"))) { // ItemBlock
				if (world->isValid(pos->add(0, -2, 0)))
					place(pos->add(0, -1, 0), EnumFacing::UP);
				else if (world->isValid(pos->add(-1, -1, 0)))
					place(pos->add(0, -1, 0), EnumFacing::EAST);
				else if (world->isValid(pos->add(1, -1, 0)))
					place(pos->add(0, -1, -1), EnumFacing::WEST);
				else if (world->isValid(pos->add(0, -1, -1)))
					place(pos->add(0, -1, 0), EnumFacing::SOUTH);
				else if (world->isValid(pos->add(0, -1, 1)))
					place(pos->add(0, -1, 0), EnumFacing::NORTH);
				else if (world->isValid(pos->add(1, -1, 1))) {
					if (world->isValid(pos->add(0, -1, 1)))
						place(pos->add(0, -1, 1), EnumFacing::NORTH);
					place(pos->add(1, -1, 1), EnumFacing::EAST);
				}
				else if (world->isValid(pos->add(-1, -1, 1))) {
					if (world->isValid(pos->add(-1, -1, 0)))
						place(pos->add(0, -1, 1), EnumFacing::WEST);
					place(pos->add(-1, -1, 1), EnumFacing::SOUTH);
				}
				else if (world->isValid(pos->add(-1, -1, -1))) {
					if (world->isValid(pos->add(0, -1, -1)))
						place(pos->add(0, -1, 1), EnumFacing::SOUTH);
					place(pos->add(-1, -1, 1), EnumFacing::WEST);
				}
				else if (world->isValid(pos->add(1, -1, -1))) {
					if (world->isValid(pos->add(1, -1, 0)))
						place(pos->add(1, -1, 0), EnumFacing::EAST);
					place(pos->add(1, -1, -1), EnumFacing::NORTH);
				}
            }
        }
    }
}

void Scaffold::onRender() {

}