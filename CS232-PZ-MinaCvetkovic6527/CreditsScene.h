#pragma once
#include "Scene.h"

class CreditsScene : public Scene {
private:
	UIButton exit_button;

	std::unique_ptr<GameObject> background;
public:
	explicit CreditsScene() {
		initialize();
	}

	void update(float delta_time) override {
		if (is_button_clicked(exit_button.box))
			switch_scene(SceneId::menu);
	}

	void render() override {
		background->render();
		exit_button.button->render();
		draw_text("Options:", 280.f, 350.f, glm::vec3(0.0f));
	}
private:
	void initialize() override {
		background = std::make_unique<GameObject>(
			"background",
			glm::vec2(0.f, 0.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));

		background->set_renderer(
			std::make_unique<SpriteRenderer>(
				"Sprites/credits.png",
				1,
				false));

		auto exit_obj = std::make_unique<GameObject>(
			"exit_button",
			glm::vec2(390.f, 200.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));

		exit_obj->set_renderer(
			std::make_unique<SpriteRenderer>(
				"Sprites/exitButton.png",
				1,
				false));

		AABB exit_box = make_aabb(
			exit_obj.get(),
			false,
			/*offsets*/{ 0,0,0,0 });

		exit_button = {
			/*button*/ std::move(exit_obj),
			/*box*/ exit_box,
			/*active*/ true };
	}
};