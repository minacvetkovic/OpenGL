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
		fit_background_to_window();
		glutSetWindowTitle("Ocean Explorer");
		layout_buttons_under_title();

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

		fit_background_to_window();
		layout_buttons_under_title();
	}

	void fit_background_to_window() {
		auto* renderer = dynamic_cast<SpriteRenderer*>(background->get_renderer());
		if (!renderer) return;

		const int texture_w = renderer->get_texture_width();
		const int texture_h = renderer->get_texture_height();
		if (texture_w <= 0 || texture_h <= 0) return;

		const float screen_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float screen_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));

		background->transform.position = glm::vec2(0.0f, 0.0f);
		background->transform.scale = glm::vec2(
			screen_width / static_cast<float>(texture_w),
			screen_height / static_cast<float>(texture_h));
	}

	void place_button(UIButton& button, float y) {
		auto* renderer = dynamic_cast<SpriteRenderer*>(button.button->get_renderer());
		if (!renderer) return;

		const float button_width = renderer->get_texture_width() * button.button->transform.scale.x;
		const float window_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));

		button.button->transform.position.x = (window_width - button_width) * 0.5f;
		button.button->transform.position.y = y;
		button.box = make_aabb(button.button.get(), false, /*offsets*/{ 0,0,0,0 });
	}

	void layout_buttons_under_title() {
		const float window_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));

		place_button(play_button, window_height * 0.42f);
		place_button(credits_button, window_height * 0.32f);
		place_button(exit_button, window_height * 0.22f);
	}
};