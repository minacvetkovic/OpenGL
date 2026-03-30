#pragma once
#include "Scene.h"

class SplashScene : public Scene {
private:
	float splash_threshold = 2.0f;
	float splash_timer = 0.0f;

	std::unique_ptr<GameObject> background;
public:
	explicit SplashScene() {
		initialize();
	}

	void update(float delta_time) override {
		splash_timer += delta_time;
		if (splash_timer >= splash_threshold) {
			switch_scene(SceneId::menu);
		}
	}

	void render() override {
		background->render();
	}

private:
	void initialize() override {
		background = std::make_unique<GameObject>(
			"splash",
			glm::vec2(0.f, 0.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));

		background->set_renderer(std::make_unique<SpriteRenderer>(
			"Sprites/splash.png",
			1,
			false));
	}
};