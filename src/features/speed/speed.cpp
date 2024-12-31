#include "../features.h"
#include "../../game/game.h"
#include "../../game/offsets.h"
#include "../../core/config.h"

namespace Features {
    void UpdateWalkingSpeed() {
        uintptr_t player = Game::GetPlayerAddress();
        if (player && Config::WalkSpeedEnabled) {
            Game::Write<float>(player + Game::Offsets::WalkSpeed, Config::WalkSpeedValue);
        }
    }

    void UpdateRunningSpeed() {
        uintptr_t player = Game::GetPlayerAddress();
        if (player && Config::RunSpeedEnabled) {
            Game::Write<float>(player + Game::Offsets::RunSpeed, Config::RunSpeedValue);
        }
    }

    void UpdateSpeedMultiplier() {
        uintptr_t world = Game::GetWorldAddress();
        if (world && Config::SpeedMultiplierEnabled) {
            Game::Write<float>(world + Game::Offsets::Multiplier, Config::SpeedMultiplierValue);
        }
    }
}
