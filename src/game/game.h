#pragma once
#include <cstdint>
#include <windows.h>
#include <iostream>

namespace SDK {
	class UWorld;
}

namespace Game {
	void Update();

	SDK::UWorld* GetWorld();

	uintptr_t GetPlayerAddress();
	uintptr_t GetWorldAddress();
	uintptr_t GetPlusUltraAddress();
	uintptr_t GetSkillAddress();

	template<typename T>
	T Read(uintptr_t address, T def = T()) {
		if (!address || IsBadReadPtr(reinterpret_cast<void*>(address), sizeof(T))) return def;
		return *reinterpret_cast<T*>(address);
	}

	template<typename T>
	void Write(uintptr_t address, T value) {
		if (!address || IsBadWritePtr(reinterpret_cast<void*>(address), sizeof(T))) return;
		*reinterpret_cast<T*>(address) = value;
	}

	namespace Player {
		bool IsValid();
	}
}
