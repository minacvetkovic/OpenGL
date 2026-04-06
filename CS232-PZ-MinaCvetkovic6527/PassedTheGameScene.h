#pragma once
#include "PassedTheScene.h"

class PassedTheGameScene : public PassedTheScene {
public:
	explicit PassedTheGameScene(int final_score = 0)
		: PassedTheScene("Sprites/PassedTheGame.png", nullptr, SceneId::menu, final_score, true, "Final Score") {
	}
};
