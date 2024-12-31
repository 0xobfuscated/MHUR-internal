#include "../features.h"
#include "../../game/game.h"
#include "../../game/offsets.h"
#include "../../core/config.h"

namespace Features {
    void UpdateJumpCount() {
        if (!Config::JumpEnabled) return;
        uintptr_t player = Game::GetPlayerAddress();
        if (player) {
            Game::Write<int>(player + Game::Offsets::JumpCount, 999);
        }
    }
}
