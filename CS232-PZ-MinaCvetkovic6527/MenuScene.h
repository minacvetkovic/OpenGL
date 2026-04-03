#pragma once
#include "Scene.h"

class MenuScene : public Scene {
private:
	UIButton play_button;
	UIButton credits_button;
	UIButton exit_button;
	UIButton level1_button;
	UIButton level2_button;

	bool show_level_select = false;
	bool using_level_menu_background = false;
	std::unique_ptr<GameObject> background;

public:
	explicit MenuScene() {
		initialize();
	}

	void update(float delta_time) override {

		update_background_for_state();
		fit_background_to_window();

		if (!show_level_select) {
			glutSetWindowTitle("Ocean Explorer");
			layout_buttons_under_title();

			if (is_button_clicked(play_button.box))
				show_level_select = true;

			if (is_button_clicked(credits_button.box))
				switch_scene(SceneId::credits);

			if (is_button_clicked(exit_button.box))
				exit(0);
		}
		else {
			glutSetWindowTitle("Choose Level");
			layout_level_buttons();

			if (is_button_clicked(level1_button.box)) {
				show_level_select = false;
				switch_scene(SceneId::game);
			}

			if (is_button_clicked(level2_button.box)) {
				show_level_select = false;
				switch_scene(SceneId::game_level2);
			}

			if (is_button_clicked(exit_button.box))
				show_level_select = false;
		}

		Input::update();
	}

	void render() override {
		background->render();

		if (!show_level_select) {
			play_button.button->render();
			credits_button.button->render();
			exit_button.button->render();
		}
		else {
			level1_button.button->render();
			level2_button.button->render();
			exit_button.button->render();
		}
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

		auto level1_obj = std::make_unique<GameObject>(
			"level1_button",
			glm::vec2(390.f, 240.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));

		level1_obj->set_renderer(
			std::make_unique<SpriteRenderer>(
				"Sprites/Level1.png",
				1,
				false));

		AABB level1_box = make_aabb(
			level1_obj.get(),
			false,
			/*offsets*/{ 0,0,0,0 });

		level1_button = {
			/*button*/ std::move(level1_obj),
			/*box*/ level1_box,
			/*active*/ true };

		auto level2_obj = std::make_unique<GameObject>(
			"level2_button",
			glm::vec2(390.f, 160.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));

		level2_obj->set_renderer(
			std::make_unique<SpriteRenderer>(
				"Sprites/Level2.png",
				1,
				false));

		AABB level2_box = make_aabb(
			level2_obj.get(),
			false,
			/*offsets*/{ 0,0,0,0 });

		level2_button = {
			/*button*/ std::move(level2_obj),
			/*box*/ level2_box,
			/*active*/ true };

		fit_background_to_window();
		layout_buttons_under_title();
	}

	void update_background_for_state() {
		if (!background) return;

		if (show_level_select && !using_level_menu_background) {
			background->set_renderer(
				std::make_unique<SpriteRenderer>(
					"Sprites/LevelMenu.png",
					1,
					false));
			using_level_menu_background = true;
		}
		else if (!show_level_select && using_level_menu_background) {
			background->set_renderer(
				std::make_unique<SpriteRenderer>(
					"Sprites/mainmenu.png",
					1,
					false));
			using_level_menu_background = false;
		}
	}

	void fit_background_to_window() {
		auto* renderer = dynamic_cast<SpriteRenderer*>(background->get_renderer());
		if (!renderer) return;

		const int texture_w = renderer->get_texture_width();
		const int texture_h = renderer->get_texture_height();
		if (texture_w <= 0 || texture_h <= 0) return;

		const float window_w = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float window_h = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));

		background->transform.position = glm::vec2(0.0f, 0.0f);
		background->transform.scale = glm::vec2(
			window_w / static_cast<float>(texture_w),
			window_h / static_cast<float>(texture_h));
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

	void layout_level_buttons() {
		const float window_h = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
		auto* level1_renderer = dynamic_cast<SpriteRenderer*>(level1_button.button->get_renderer());
		auto* level2_renderer = dynamic_cast<SpriteRenderer*>(level2_button.button->get_renderer());
		if (!level1_renderer || !level2_renderer) return;

		const float window_w = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float w1 = level1_renderer->get_texture_width() * level1_button.button->transform.scale.x;
		const float w2 = level2_renderer->get_texture_width() * level2_button.button->transform.scale.x;
		const float y = window_h * 0.33f;
		const float spacing = 25.0f;
		const float total_w = w1 + spacing + w2;
		const float start_x = (window_w - total_w) * 0.5f;

		level1_button.button->transform.position = glm::vec2(start_x, y);
		level2_button.button->transform.position = glm::vec2(start_x + w1 + spacing, y);
		level1_button.box = make_aabb(level1_button.button.get(), false, /*offsets*/{ 0,0,0,0 });
		level2_button.box = make_aabb(level2_button.button.get(), false, /*offsets*/{ 0,0,0,0 });

		place_button(exit_button, window_h * 0.16f);
	}
};