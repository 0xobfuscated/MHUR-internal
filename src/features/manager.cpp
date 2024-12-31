#include "manager.h"
#include "../game/game.h"
#include "features.h"

namespace FeatureManager {
	void Update() {
		Game::Update();

		Features::RenderESP();

		if (!Game::Player::IsValid()) {
			return;
		}

		Features::UpdateWalkingSpeed();
		Features::UpdateRunningSpeed();
		Features::UpdateSpeedMultiplier();

		Features::UpdateJumpCount();
		Features::UpdatePlusUltra();
		Features::UpdateSkillsCooldown();

		Features::UpdateAimbot();
		Features::RenderAimbotFOV();
	}
}
