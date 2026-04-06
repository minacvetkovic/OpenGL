#pragma once
#include "Scene.h"

class OptionsScene : public Scene {
private:
	UIButton exit_button;

	std::unique_ptr<GameObject> background;
public:
	explicit OptionsScene() {
		initialize();
	}

	void update(float delta_time) override {
		fit_background_to_window();
		layout_ui();

		if (is_button_clicked(exit_button.box))
			switch_scene(SceneId::menu);
	}

	void render() override {
		background->render();
		exit_button.button->render();

		const float screen_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float screen_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
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
				"Sprites/options.png",
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

		fit_background_to_window();
		layout_ui();
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

	void layout_ui() {
		auto* renderer = dynamic_cast<SpriteRenderer*>(exit_button.button->get_renderer());
		if (!renderer) return;

		const float screen_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float screen_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
		const float button_w = renderer->get_texture_width() * exit_button.button->transform.scale.x;
		const float button_h = renderer->get_texture_height() * exit_button.button->transform.scale.y;
		const float right_margin = 20.0f;
		const float bottom_margin = 20.0f;

		exit_button.button->transform.position.x = screen_width - button_w - right_margin;
		exit_button.button->transform.position.y = bottom_margin;
		exit_button.box = make_aabb(exit_button.button.get(), false, /*offsets*/{ 0,0,0,0 });
	}
};