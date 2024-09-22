#pragma once

#include "MinecraftInstance.h"

#include <iostream>
#include <vector>

class Module {
public:
	bool isEnabled;
	const char* moduleName;
	const char* moduleDesc;

	Module(const char* name, const char* desc) {
		moduleName = name;
		moduleDesc = desc;
		isEnabled = false;
	};

	virtual void Enable() {

	}

	virtual void Disable() {

	}

	virtual void onTick() {

	}

	virtual void onRender() {

	}

	virtual ~Module() {};
};