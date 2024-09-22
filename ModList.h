#pragma once

#include "VM.h"
#include "Module.h"
#include "PlayerUtils.h"
#include "RainbowUtil.h"
#include "TextRenderer.h"

#include <iostream>
#include <vector>
#include <math.h>
#include <chrono>

// raindbow strip data

class ModList {
private:
public:
	static void drawList(std::vector<std::unique_ptr<Module>>& modules) {
		int x = -PlayerUtils::width / 2.0F + 10;
		int y = -PlayerUtils::height / 2.0F + 25;

        float offset = 0;

        for (size_t i = 0; i < modules.size(); ++i) {
            auto& mod = modules[i];
            if (mod->isEnabled) {
                float r, g, b;
                RainbowUtil::get(0, r, g, b, 0.5F);

                TextRenderer::drawWithShadow(x, y, mod->moduleName, new float[4] { r, g, b, 1 });
                y += 20;
            }
        }
	};
};