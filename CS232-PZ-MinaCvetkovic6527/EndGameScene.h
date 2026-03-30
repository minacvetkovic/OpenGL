#pragma once
#include "Scene.h"

class EndGameScene : public Scene {
private:
	UIButton replay_button;
	UIButton exit_button;

	int score = 0;

	std::unique_ptr<GameObject> background;
public:
	explicit EndGameScene(int new_score) : score(new_score) {
		initialize();
	}

	void update(float delta_time) override {
		if (is_button_clicked(exit_button.box))
			switch_scene(SceneId::menu);

		if (is_button_clicked(replay_button.box))
			switch_scene(SceneId::game);
	}

	void render() override {
		background->render();

		exit_button.button->render();
		replay_button.button->render();

		std::string text = "Score: " + std::to_string(score);
		draw_text(text.c_str(), 370.f, 340.f, glm::vec3(0.0f));
	}

private:
	void initialize() override {
		background = std::make_unique<GameObject>(
			"background",
			glm::vec2(0.f, 0.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(1.f));

		background->set_renderer(
			std::make_unique<SpriteRenderer>(
				"Sprites/gameover.png",
				1,
				false));

		auto replay_obj = std::make_unique<GameObject>(
			"replay_button",
			glm::vec2(390.f, 260.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));

		replay_obj->set_renderer(
			std::make_unique<SpriteRenderer>(
				"Sprites/restartButton.png",
				1,
				false));

		AABB replay_box = make_aabb(
			replay_obj.get(),
			false,
			/*offsets*/{ 0,0,0,0 });

		replay_button = {
			/*button*/ std::move(replay_obj),
			/*box*/ replay_box,
			/*active*/ true };

		auto exit_obj = std::make_unique<GameObject>(
			"exit_button",
			glm::vec2(390.f, 180.f),
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