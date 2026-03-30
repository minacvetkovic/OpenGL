#pragma once
#include "Scene.h"

class MenuScene : public Scene {
private:
	UIButton play_button;
	UIButton credits_button;
	UIButton exit_button;

	std::unique_ptr<GameObject> background;

public:
	explicit MenuScene() {
		initialize();
	}

	void update(float delta_time) override {

		if (is_button_clicked(play_button.box))
			switch_scene(SceneId::game);

		if (is_button_clicked(credits_button.box))
			switch_scene(SceneId::credits);

		if (is_button_clicked(exit_button.box))
			exit(0);

		Input::update();
	}

	void render() override {
		background->render();
		play_button.button->render();
		credits_button.button->render();
		exit_button.button->render();
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
				"Sprites/mainmenu.png",
				1,
				false));

		auto play_obj = std::make_unique<GameObject>(
			"play_button",
			glm::vec2(390.f, 280.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));

		play_obj->set_renderer(
			std::make_unique<SpriteRenderer>(
				"Sprites/playButton.png",
				1,
				false));

		AABB play_box = make_aabb(
			play_obj.get(),
			false,
			/*offsets*/{ 0,0,0,0 });

		play_button = {
			/*button*/ std::move(play_obj),
			/*box*/ play_box,
			/*active*/ true };

		auto credits_obj = std::make_unique<GameObject>(
			"option_button",
			glm::vec2(390.f, 200.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));

		credits_obj->set_renderer(
			std::make_unique<SpriteRenderer>(
				"Sprites/optionButton.png",
				1,
				false));

		AABB credits_box = make_aabb(
			credits_obj.get(),
			false,
			/*offsets*/{ 0,0,0,0 });

		credits_button = {
			/*button*/ std::move(credits_obj),
			/*box*/ credits_box,
			/*active*/ true };

		auto exit_obj = std::make_unique<GameObject>(
			"exit_button",
			glm::vec2(390.f, 120.f),
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