#include "../features.h"
#include "../../game/game.h"
#include "../../game/offsets.h"
#include "../../core/config.h"

namespace Features {
    void UpdateSkillsCooldown() {
        uintptr_t skill = Game::GetSkillAddress();
        if (skill && Config::SkillEnabled) {
            Game::Write<float>(skill + Game::Offsets::SkillsCooldown, Config::SkillsCooldownValue);
        }
    }
}
