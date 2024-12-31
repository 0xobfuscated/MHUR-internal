#include "hooks.h"
#include "../../../ext/minhook/minhook.h"
#include "../utils/console.h"
#include <cstdio>

namespace Hooks {
    bool Init() {
        return MH_Initialize() == MH_OK;
    }

    void Shutdown() {
        MH_Uninitialize();
    }

    bool Create(void* target, void* detour, void** original) {
        if (MH_CreateHook(target, detour, original) != MH_OK) {
            LOG(LOG_ERROR, "Failed to create hook at %p", target);
            return false;
        }
        return true;
    }

    bool Enable(void* target) {
        return MH_EnableHook(target) == MH_OK;
    }

    bool Disable(void* target) {
        return MH_DisableHook(target) == MH_OK;
    }

    void Remove(void* target) {
        MH_RemoveHook(target);
    }
}
