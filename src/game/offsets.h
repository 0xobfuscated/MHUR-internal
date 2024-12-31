#pragma once
#include <cstdint>

namespace Game {
	namespace Offsets {
        constexpr int32_t GWorld = 0x06B0DB50;
        constexpr int32_t GObjects = 0x069C1090;

		constexpr uintptr_t BaseObject = 0x06AF6160;

		constexpr uintptr_t WalkSpeed = 0xE0;
		constexpr uintptr_t RunSpeed = 0xF8;
		constexpr uintptr_t Multiplier = 0x98;

		constexpr uintptr_t JumpCount = 0x150;
		constexpr uintptr_t PlusUltra = 0x970;
		constexpr uintptr_t SkillsCooldown = 0xB4;
	}
}
