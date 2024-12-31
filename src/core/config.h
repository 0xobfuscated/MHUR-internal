#pragma once

namespace Config {
    inline bool WalkSpeedEnabled = false;
    inline bool RunSpeedEnabled = false;
    inline bool SpeedMultiplierEnabled = false;
    inline bool JumpEnabled = false;

    inline float WalkSpeedValue = 600.0f;
    inline float RunSpeedValue = 800.0f;
    inline float SpeedMultiplierValue = 1.0f;
    
    inline bool PlusUltraEnabled = false;
    inline bool SkillEnabled = false;
    
    inline float PlusUltraValue = 100.0f;
    inline float SkillsCooldownValue = 100.0f; // 100 = fully charged

    inline bool EspEnabled = false;
    inline bool EspBox = false;
    inline bool EspSkeleton = false;
    inline bool EspSnapline = false;
    inline bool EspDistance = false;
    inline bool EspName = false;
    
    // line pos: 0 = Bottom, 1 = Top
    inline int espSnaplinePosition = 0;

    inline bool EspBoxRainbow = true;
    inline bool EspSkeletonRainbow = true;
    inline bool EspSnaplineRainbow = true;

    inline float espBoxColor[3] = { 1.0f, 0.0f, 0.0f };
    inline float espSkeletonColor[3] = { 1.0f, 0.0f, 0.0f };
    inline float espSnaplineColor[3] = { 1.0f, 0.0f, 0.0f };

    inline bool AimbotEnabled = false;
    inline bool AimbotDrawFOV = false;
    inline float AimbotFOV = 100.0f;
    inline float AimbotSmoothness = 5.0f;
    inline int AimbotKey = 0x02; // VK_RBUTTON

    inline float EspMaxDistance = 10000.0f;
}
