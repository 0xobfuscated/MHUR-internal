#include "../features.h"
#include "../../game/game.h"
#include "../../game/offsets.h"
#include "../../core/config.h"

namespace Features {
    void UpdatePlusUltra() {
        if (!Config::PlusUltraEnabled) return;
        uintptr_t plusUltra = Game::GetPlusUltraAddress();
        if (plusUltra) {
            Game::Write<float>(plusUltra + Game::Offsets::PlusUltra, Config::PlusUltraValue);
        }
    }
}
