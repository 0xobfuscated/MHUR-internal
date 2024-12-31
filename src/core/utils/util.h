#pragma once
#include <Windows.h>
#include "../../../ext/imgui/imgui.h"

namespace Utils {

void MouseMove(float target_x, float target_y, float screen_width, float screen_height, float smoothness);
void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);
ImU32 GetRainbowColor();
ImU32 ColorToImU32(const float rgb[3], float alpha = 1.0f);

}
