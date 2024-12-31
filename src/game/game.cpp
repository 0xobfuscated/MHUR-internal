#include "game.h"
#include "offsets.h"
#include "../core/utils/memory.h"
#include "sdk.h"
#include <vector>
#include <Windows.h>

namespace Game {
	namespace Internal {
		const std::vector<uintptr_t> PlayerChain = { 0x30, 0x228, 0x280, 0x828 };
		const std::vector<uintptr_t> MultiplierChain = { 0x30, 0x228, 0x280 };
		const std::vector<uintptr_t> PlusUltraChain = { 0x30, 0x228 };
		const std::vector<uintptr_t> SkillChain = { 0x30, 0x228, 0xC18 };

		uintptr_t CachedPlayer = 0;
		uintptr_t CachedWorld = 0;
		uintptr_t CachedPlusUltra = 0;
		uintptr_t CachedSkill = 0;

		uintptr_t ResolveChain(const std::vector<uintptr_t>& chain) {
			uintptr_t baseAddress = Memory::GetImageBase();
			if (!baseAddress) return 0;

			uintptr_t address = baseAddress + Offsets::BaseObject;
			if (IsBadReadPtr(reinterpret_cast<void*>(address), sizeof(uintptr_t))) return 0;

			address = *reinterpret_cast<uintptr_t*>(address);
			if (!address) return 0;

			for (uintptr_t offset : chain) {
				address += offset;
				if (IsBadReadPtr(reinterpret_cast<void*>(address), sizeof(uintptr_t))) return 0;

				address = *reinterpret_cast<uintptr_t*>(address);
				if (!address) return 0;
			}

			return address;
		}
	}

	SDK::UWorld* GetWorld() {
		return *reinterpret_cast<SDK::UWorld**>(Memory::GetImageBase() + Game::Offsets::GWorld);
	}

	void Update() {
		Internal::CachedPlayer = Internal::ResolveChain(Internal::PlayerChain);
		Internal::CachedWorld = Internal::ResolveChain(Internal::MultiplierChain);
		Internal::CachedPlusUltra = Internal::ResolveChain(Internal::PlusUltraChain);
		Internal::CachedSkill = Internal::ResolveChain(Internal::SkillChain);
	}

	uintptr_t GetPlayerAddress() { 
		return Internal::CachedPlayer; 
	}
	
	uintptr_t GetWorldAddress() {
		return Internal::CachedWorld; 
	}

	uintptr_t GetPlusUltraAddress() {
		return Internal::CachedPlusUltra; 
	}

	uintptr_t GetSkillAddress() {
		return Internal::CachedSkill; 
	}

	namespace Player {
		bool IsValid() { 
			return Internal::CachedPlayer != 0; 
		}
	}
}
