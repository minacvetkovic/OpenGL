#pragma once
#include "PassedTheScene.h"

class PassedTheLevelScene : public PassedTheScene {
public:
	explicit PassedTheLevelScene(int score = 0)
		: PassedTheScene("Sprites/PassedTheLevel.png", "Sprites/Level2.png", SceneId::game_level2, score, true, "Score") {
		set_layout(0.08f, 0.26f, 0.14f);
	}
};