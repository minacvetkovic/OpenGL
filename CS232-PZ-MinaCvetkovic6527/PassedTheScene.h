#pragma once
#include "Scene.h"
#include <string>

class PassedTheScene : public Scene {
private:
	std::string background_file;
	std::string primary_button_file;
	std::string score_label = "Final Score";
	SceneId primary_scene = SceneId::menu;
	bool has_primary_button = false;
	int score = 0;
	bool show_score = false;
	float score_y_ratio = 0.33f;
	float primary_button_y_ratio = 0.32f;
	float exit_button_y_ratio = 0.20f;

	UIButton primary_button;
	UIButton exit_button;
	std::unique_ptr<GameObject> background;

public:
	explicit PassedTheScene(
		const char* background_path,
		const char* primary_button_path = nullptr,
		SceneId next_scene = SceneId::menu,
		int final_score = 0,
		bool draw_score = false,
		const char* score_text = "Final Score",
		float score_y = 0.33f,
		float primary_button_y = 0.32f,
		float exit_button_y = 0.20f)
		: background_file(background_path),
		  primary_scene(next_scene),
		  has_primary_button(primary_button_path != nullptr),
		  score(final_score),
		  show_score(draw_score),
		  score_label(score_text),
		  score_y_ratio(score_y),
		  primary_button_y_ratio(primary_button_y),
		  exit_button_y_ratio(exit_button_y) {
		if (primary_button_path) primary_button_file = primary_button_path;
		initialize();
	}

	void update(float delta_time) override {
		fit_background_to_window();
		layout_ui();

		if (has_primary_button && is_button_clicked(primary_button.box))
			switch_scene(primary_scene);

		if (is_button_clicked(exit_button.box))
			switch_scene(SceneId::menu);
	}

	void render() override {
		background->render();
		if (has_primary_button) primary_button.button->render();
		exit_button.button->render();

		if (!show_score) return;

		const std::string score_text = score_label + ": " + std::to_string(score);
		const float screen_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float screen_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
		draw_text(score_text.c_str(), centered_text_x(score_text, screen_width), screen_height * score_y_ratio);
	}

protected:
	void set_layout(float score_y, float primary_button_y, float exit_button_y) {
		score_y_ratio = score_y;
		primary_button_y_ratio = primary_button_y;
		exit_button_y_ratio = exit_button_y;
	}

private:
	void initialize() override {
		background = std::make_unique<GameObject>(
			"background",
			glm::vec2(0.f, 0.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));
		background->set_renderer(std::make_unique<SpriteRenderer>(background_file.c_str(), 1, false));

		if (has_primary_button) {
			auto primary_obj = std::make_unique<GameObject>(
				"primary_button",
				glm::vec2(0.f, 0.f),
				0.f,
				glm::vec2(1.f),
				glm::vec2(0.f));
			primary_obj->set_renderer(std::make_unique<SpriteRenderer>(primary_button_file.c_str(), 1, false));
			primary_button = { std::move(primary_obj), {}, true };
		}

		auto exit_obj = std::make_unique<GameObject>(
			"exit_button",
			glm::vec2(0.f, 0.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f));
		exit_obj->set_renderer(std::make_unique<SpriteRenderer>("Sprites/exitButton.png", 1, false));
		exit_button = { std::move(exit_obj), {}, true };

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

	void place_button(UIButton& button, float y) {
		auto* renderer = dynamic_cast<SpriteRenderer*>(button.button->get_renderer());
		if (!renderer) return;

		const float button_w = renderer->get_texture_width() * button.button->transform.scale.x;
		const float screen_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		button.button->transform.position.x = (screen_width - button_w) * 0.5f;
		button.button->transform.position.y = y;
		button.box = make_aabb(button.button.get(), false, { 0,0,0,0 });
	}

	void layout_ui() {
		const float screen_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
		if (has_primary_button) place_button(primary_button, screen_height * primary_button_y_ratio);
		place_button(exit_button, screen_height * exit_button_y_ratio);
	}

	float text_width(const std::string& text) {
		float width = 0.0f;
		for (char c : text) width += glutStrokeWidth(GLUT_STROKE_ROMAN, c) * 0.2f;
		return width;
	}

	float centered_text_x(const std::string& text, float screen_width) {
		return (screen_width - text_width(text)) * 0.5f;
	}
};