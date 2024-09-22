#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <chrono>

class RainbowUtil {
public:
    static void HSBtoRGB(float h, float s, float br, float& r, float& g, float& b) {
        float chroma = br * s;
        float h_prime = h * 6.0f;
        float x = chroma * (1.0f - std::fabs(fmod(h_prime, 2.0f) - 1.0f));

        float r1 = 0.0f, g1 = 0.0f, b1 = 0.0f;
        if (h_prime < 1.0f) {
            r1 = chroma;
            g1 = x;
            b1 = 0.0f;
        }
        else if (h_prime < 2.0f) {
            r1 = x;
            g1 = chroma;
            b1 = 0.0f;
        }
        else if (h_prime < 3.0f) {
            r1 = 0.0f;
            g1 = chroma;
            b1 = x;
        }
        else if (h_prime < 4.0f) {
            r1 = 0.0f;
            g1 = x;
            b1 = chroma;
        }
        else if (h_prime < 5.0f) {
            r1 = x;
            g1 = 0.0f;
            b1 = chroma;
        }
        else {
            r1 = chroma;
            g1 = 0.0f;
            b1 = x;
        }

        float m = br - chroma;
        r = static_cast<float>((r1 + m));
        g = static_cast<float>((g1 + m));
        b = static_cast<float>((b1 + m));
    }

	static void get(float i, float &r, float &g, float &b, float speed) {
        int state = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() / 12) % 360;

        // Apply offset based on the module index
        float offset = static_cast<float>(i) * speed; // Adjust the offset multiplier as needed
        float hue = static_cast<float>(state + (speed * 360)) / 360.0f;

        if (hue > 1.0f) {
            hue -= 1.0f; // Wrap around
        }
       
        HSBtoRGB(hue, 0.8f, 1.0f, r, g, b);
	}
};