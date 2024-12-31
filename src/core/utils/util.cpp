#include "util.h"
#include <chrono>

namespace Utils {

void MouseMove(float target_x, float target_y, float screen_width, float screen_height, float smoothness) {
    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    float delta_x = target_x - center_x;
    float delta_y = target_y - center_y;

    if (smoothness < 3.0f) smoothness = 3.0f;

    if (smoothness > 0.0f) {
        delta_x /= smoothness;
        delta_y /= smoothness;
    }

    mouse_event(MOUSEEVENTF_MOVE, (DWORD)(int)delta_x, (DWORD)(int)delta_y, 0, 0);
}

void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
    int i = static_cast<int>(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
}

ImU32 GetRainbowColor() {
    static auto startTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - startTime).count();
    
    float hue = fmodf(elapsed / 1.5f, 1.0f);
    float r, g, b;
    HSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
    
    return IM_COL32(
        static_cast<int>(r * 255),
        static_cast<int>(g * 255),
        static_cast<int>(b * 255),
        255
    );
}

ImU32 ColorToImU32(const float rgb[3], float alpha) {
    return IM_COL32(
        static_cast<int>(rgb[0] * 255),
        static_cast<int>(rgb[1] * 255),
        static_cast<int>(rgb[2] * 255),
        static_cast<int>(alpha * 255)
    );
}

}
