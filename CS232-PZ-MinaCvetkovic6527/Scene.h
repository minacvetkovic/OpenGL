#pragma once
#include "glut.h"
#include "Input.h"
#include <unordered_map>
#include "CollisionUtils.h"
#include "SoundManager.h"
#include "UIButton.h"

class Scene
{
public:
	explicit Scene() = default;
	virtual ~Scene() = default;

	virtual void initialize() = 0;
	virtual void update(float delta_time) = 0;
	virtual void render() = 0;

	void draw_text(const char* text,
		float x, float y,
		glm::vec3 color = glm::vec3(0.0f),
		float scale = 0.2,
		float weight_px = 1.0) {
		glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glLineWidth(weight_px);

		glColor3f(color.r, color.g, color.b);
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		glScalef(scale, scale, scale);

		for (const char* c = text; *c; ++c) {
			glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
		}

		glPopMatrix();

		glPopAttrib();
		glEnable(GL_TEXTURE_2D);
	}

	bool is_button_clicked(const AABB& button) {
		if (Input::get_mouse_button_down(GLUT_LEFT_BUTTON)) {
			float screen_x, screen_y;
			Input::get_mouse_screen_position(&screen_x, &screen_y);

			float gy = float(glutGet(GLUT_WINDOW_HEIGHT)) - screen_y;

			if (point_in_aabb(button, screen_x, gy)) {
				SoundManager::get_instance().audio_manager->playSound(
					SoundManager::get_instance().button_sound,
					nullptr, false,
					&SoundManager::get_instance().surrounding_sounds
				);
				return true;
			}
		}

		return false;
	}
};

enum class SceneId { game, menu, game_over, credits, spash };

struct SceneEntry {
	std::unique_ptr<Scene> ptr;
	bool active = false;
};

extern std::unordered_map<SceneId, SceneEntry> scenes;
extern Scene* current_scene;

inline void switch_scene(SceneId id) {
	auto it = scenes.find(id);
	if (it == scenes.end() || !it->second.ptr) return;

	for (auto& kv : scenes) kv.second.active = false;

	it->second.active = true;
	current_scene = it->second.ptr.get();
}