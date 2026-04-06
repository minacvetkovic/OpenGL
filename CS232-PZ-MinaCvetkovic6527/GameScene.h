#pragma once
#include "Scene.h"
#include "Constants.h"
#include <string>
#include "EndGameScene.h"
#include "PassedTheLevelScene.h"
#include "PassedTheGameScene.h"

class GameScene : public Scene {

private:
	std::unique_ptr<GameObject> background;
	std::unique_ptr<GameObject> player;

	std::vector<std::unique_ptr<GameObject>> pickups;

	int score = 0;
	float pickup_timer = 0.0f;
	float level_timer = 0.0f;
	float bad_pickup_chance = 0.5f;
	SceneId this_scene_id = SceneId::game;
	bool shark_hit = false;

	UIButton exit_button;
public:
	explicit GameScene(float bad_spawn_chance = 0.5f, SceneId scene_id = SceneId::game)
		: bad_pickup_chance(bad_spawn_chance), this_scene_id(scene_id) {
		initialize();
	}

	void update(float delta_time) override {

		fit_background_to_window();
		layout_top_buttons();
		shark_hit = false;

		if (is_button_clicked(exit_button.box)) {
			switch_scene(SceneId::menu);
			restart_game();
			return;
		}

		check_background();
		move_player(delta_time);
		check_edges_for_player();

		level_timer += delta_time;
		if (level_timer >= GameConstants::LEVEL_THRESHOLD) {
			if (score >= 300) {
				if (this_scene_id == SceneId::game) {
					level_passed();
				}
				else if (this_scene_id == SceneId::game_level2) {
					game_passed();
				}
			}
			else {
				game_over();
			}
			return;
		}

		background->update(delta_time);
		player->update(delta_time);

		if (should_generate_pickups()) {
			pickup_timer += delta_time;
			const float fast_spawn_interval = GameConstants::PICKUPS_SPAWN_INTERVAL * 0.5f;
			if (pickup_timer >= fast_spawn_interval) {
				generate_pickup();
				pickup_timer = 0.0f;
			}
		}

		for (auto it = pickups.begin(); it != pickups.end();) {
			if (check_for_collision(it->get())) {
				it = pickups.erase(it);
			}
			else {
				(*it)->update(delta_time);
				if (is_pickup_out_of_screen(it->get())) {
					it = pickups.erase(it);
				}
				else {
					++it;
				}
			}
		}

		if (shark_hit || score <= -100) {
			game_over();
			return;
		}
	}

	void render() override {
		background->render();

		for (const auto& pickup : pickups) {
			pickup->render();
		}

		player->render();

		exit_button.button->render();

		const std::string score_text = "Score: " + std::to_string(score);
		const float screen_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float screen_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));

		float text_w = 0.0f;
		for (char c : score_text) {
			text_w += glutStrokeWidth(GLUT_STROKE_ROMAN, c) * 0.2f;
		}

		const float score_x = (screen_width - text_w) * 0.5f;
		const float score_y = screen_height - 40.0f;
		draw_text(score_text.c_str(), score_x, score_y);

		float remaining_time = GameConstants::LEVEL_THRESHOLD - level_timer;
		if (remaining_time < 0.0f) remaining_time = 0.0f;

		int time_left = static_cast<int>(remaining_time);
		if (remaining_time > static_cast<float>(time_left)) {
			++time_left;
		}

		const std::string timer_text = "Time: " + std::to_string(time_left);
		float timer_w = 0.0f;
		for (char c : timer_text) {
			timer_w += glutStrokeWidth(GLUT_STROKE_ROMAN, c) * 0.2f;
		}

		const float timer_x = screen_width - timer_w - 20.0f;
		const float timer_y = screen_height - 40.0f;
		draw_text(timer_text.c_str(), timer_x, timer_y);
	}

private:
	void initialize() override {
		const char* background_file = (this_scene_id == SceneId::game_level2)
			? "Sprites/background2.png"
			: "Sprites/background.png";

		background = std::make_unique<GameObject>(
			/*name*/ "background",
			/*position*/ glm::vec2(0.0f, 0.0f),
			/*rotation_deg*/ 0.0f,
			/*scale*/ glm::vec2(1.0f),
			/*velocity*/ glm::vec2(-50.0f, 0.0f)
		);
		background->set_renderer(
			std::make_unique<SpriteRenderer>(
				/*file_name*/ background_file,
				/*number_of_frames*/ 1,
				/*use_transparency*/ false)
		);

		player = std::make_unique<GameObject>(
			/*name*/ "player",
			/*position*/ glm::vec2(0.0f, 0.0f),
			/*rotation_deg*/ 0.0f,
			/*scale*/ glm::vec2(0.4f),
			/*velocity*/ glm::vec2(0.0)
		);
		player->set_renderer(
			std::make_unique<SpriteRenderer>(
				/*file_name*/ "Sprites/robot_right_strip.png",
				/*number_of_frames*/ 1,
				/*use_transparency*/ true)
		);

		auto exit_obj = std::make_unique<GameObject>(
			"exit_button",
			glm::vec2(150.f, 550.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f)
		);

		exit_obj->set_renderer(std::make_unique<SpriteRenderer>(
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

		layout_top_buttons();
	}

	void layout_top_buttons() {
		const float window_h = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
		const float top_margin = 10.0f;
		const float left_margin = 20.0f;

		auto* exit_renderer = dynamic_cast<SpriteRenderer*>(exit_button.button->get_renderer());
		if (!exit_renderer) return;

		const float exit_h = exit_renderer->get_texture_height() * exit_button.button->transform.scale.y;

		exit_button.button->transform.position = glm::vec2(left_margin, window_h - exit_h - top_margin);
		exit_button.box = make_aabb(exit_button.button.get(), false, /*offsets*/{ 0,0,0,0 });
	}

	void fit_background_to_window() {
		auto* renderer = get_background_renderer();
		if (!renderer) return;

		const int texture_w = renderer->get_texture_width();
		const int texture_h = renderer->get_texture_height();
		if (texture_w <= 0 || texture_h <= 0) return;

		const float screen_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float screen_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));

		const float min_world_width = screen_width * 1.8f;
		const float scale_w = min_world_width / static_cast<float>(texture_w);
		const float scale_y = screen_height / static_cast<float>(texture_h);
		const float fill_scale = (scale_w > scale_y) ? scale_w : scale_y;

		background->transform.scale = glm::vec2(fill_scale);
	}

	SpriteRenderer* get_player_renderer() {
		return dynamic_cast<SpriteRenderer*>(player->get_renderer());
	}

	SpriteRenderer* get_background_renderer() {
		return dynamic_cast<SpriteRenderer*>(background->get_renderer());
	}

	AABB get_player_offset() {
		auto* renderer = get_player_renderer();
		return (renderer && renderer->is_x_flipped())
			? PlayerOffsets::FACING_LEFT
			: PlayerOffsets::FACING_RIGHT;
	}

	AABB get_player_bounding_box() {
		const AABB base = make_aabb(player.get(), true, get_player_offset());
		return expand_aabb(base, -30.0f);
	}

	AABB get_pickup_bounding_box(GameObject* pickup) {
		const bool is_shark = (std::strcmp(pickup->get_name(), "shark") == 0);
		if (is_shark) {
			const AABB base = make_aabb(pickup, false, PickupOffsets::DEFAULT);
			return expand_aabb(base, -50.0f);
		}
		return make_aabb(pickup, false, PickupOffsets::DEFAULT);
	}

	bool should_generate_pickups() {
		if (GameConstants::MAX_ACTIVE_PICKUPS == pickups.size()) {
			bool good_pickup = false;
			for (auto& pickup : pickups) {
				if (std::strcmp(pickup->get_name(), "good pickup") == 0) return false;
			}

			if (!good_pickup) pickups.clear();
		}
		return true;
	}

	void check_background() {
		auto* bg_renderer = get_background_renderer();
		auto* player_renderer = get_player_renderer();
		if (!bg_renderer || !player_renderer) return;

		const float window_width = glutGet(GLUT_WINDOW_WIDTH);
		const float background_width = bg_renderer->get_texture_width() * background->transform.scale.x;
		const float left_edge = 0.0f;
		const float right_edge = window_width - background_width;

		float& x = background->transform.position.x;
		x = glm::clamp(x, right_edge, left_edge);

		const bool at_left_edge = (x >= left_edge && background->transform.velocity.x > 0 && !player_renderer->is_x_flipped());
		const bool at_right_edge = (x <= right_edge && background->transform.velocity.x < 0 && player_renderer->is_x_flipped());

		if (at_left_edge || at_right_edge) {
			background->transform.velocity = glm::vec2(0.0f);
		}
	}

void check_edges_for_player() {
		auto* player_renderer = get_player_renderer();
		if (!player_renderer) return;

		const float window_width = glutGet(GLUT_WINDOW_WIDTH);
		const float player_width = player_renderer->get_frame_width() * player->transform.scale.x;
		const float left_edge = 0.0f;
		const float right_edge = window_width - player_width;

		float& x = player->transform.position.x;
		x = glm::clamp(x, left_edge, right_edge);

		if (x <= left_edge && player->transform.velocity.x < 0.0f) {
			player->transform.velocity.x = 0.0f;
		}
		if (x >= right_edge && player->transform.velocity.x > 0.0f) {
			player->transform.velocity.x = 0.0f;
		}
	}

	void set_player_movement(bool face_right) {
		auto* renderer = get_player_renderer();
		if (!renderer) return;

		renderer->horizontal_flip(!face_right);

		const float player_dir = face_right ? 1.0f : -1.0f;
		const float bg_dir = -player_dir;

		player->transform.velocity = glm::vec2(player_dir * GameConstants::PLAYER_SPEED, 0.0f);
		background->transform.velocity = glm::vec2(bg_dir * GameConstants::BACKGROUND_SPEED, 0.0f);
	}

	void move_player(float delta_time) {
		auto* player_renderer = get_player_renderer();
		if (!player_renderer) return;

		const float window_height = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
		const float player_height = player_renderer->get_texture_height() * player->transform.scale.y;
		const float min_y = GameConstants::PLAYER_LOWER_POSITION;
		const float max_y = window_height - player_height;

		const float vertical_speed = 380.0f;
		float old_y = player->transform.position.y;

		if (Input::get_key('W')) {
			player->transform.position.y += vertical_speed * delta_time;
		}
		else if (Input::get_key('S')) {
			player->transform.position.y -= vertical_speed * delta_time;
		}

		player->transform.position.y = glm::clamp(player->transform.position.y, min_y, max_y);

		if ((Input::get_key_down('W') || Input::get_key_down('S')) && player->transform.position.y != old_y) {
			SoundManager::get_instance().play_jump();
		}

		if (Input::get_key_down('A')) {
			set_player_movement(false);
		}
		else if (Input::get_key_down('D')) {
			set_player_movement(true);
		}
	}

	bool is_position_valid(const glm::vec2& position, GameObject* pickup, const AABB& safe_zone) {
		pickup->transform.position = position;
		const AABB pickup_bb = get_pickup_bounding_box(pickup);

		if (intersects(safe_zone, pickup_bb)) {
			return false;
		}

		for (const auto& existing_pickup : pickups) {
			const AABB existing_bb = get_pickup_bounding_box(existing_pickup.get());
			if (intersects(pickup_bb, existing_bb)) {
				return false;
			}
		}

		return true;
	}

	float random_pickup_y(float pickup_height) {
		const float window_h = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));

		const float top_ui_reserved = 120.0f;
		const float min_y = window_h * 0.25f;
		const float max_y = glm::max(min_y, window_h - top_ui_reserved - pickup_height);

		const int max_range = static_cast<int>(max_y - min_y);
		if (max_range <= 0) return min_y;

		return min_y + static_cast<float>(std::rand() % (max_range + 1));
	}

	float random_left_y(float pickup_height) {
		const float window_h = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
		const float min_y = GameConstants::PLAYER_LOWER_POSITION;
		const float max_y = glm::max(min_y, window_h - pickup_height);

		const int max_range = static_cast<int>(max_y - min_y);
		if (max_range <= 0) return min_y;

		return min_y + static_cast<float>(std::rand() % (max_range + 1));
	}

	float random_right_y(float pickup_height) {
		return random_left_y(pickup_height);
	}

	const char* get_bad_pickup_texture(bool& shark_spawn, bool& can_spawn) {
		if (this_scene_id == SceneId::game_level2) {
			const int pick = std::rand() % 3;
			shark_spawn = (pick == 1);
			can_spawn = (pick == 2);
			static const char* bad_textures[] = {
				"Sprites/piranha.png",
				"Sprites/shark.png",
				"Sprites/can.png"
			};
			return bad_textures[pick];
		}

		shark_spawn = (std::rand() % 2) == 1;
		can_spawn = false;
		return shark_spawn ? "Sprites/shark.png" : "Sprites/piranha.png";
	}

	const char* get_good_pickup_texture(bool& nemo_spawn) {
		static const char* good_textures[] = {
			"Sprites/pearl.png",
			"Sprites/cancer.png",
			"Sprites/nemo.png"
		};

		if (this_scene_id == SceneId::game_level2) {
			const int pick = std::rand() % 3;
			nemo_spawn = (pick == 2);
			return good_textures[pick];
		}

		nemo_spawn = false;
		return good_textures[std::rand() % 2];
	}

	bool spawn_side(
		std::unique_ptr<GameObject>& pickup,
		bool should_spawn,
		bool from_left,
		float pickup_height,
		float pickup_width
	) {
		if (!should_spawn) return false;

		const float y = from_left ? random_left_y(pickup_height) : random_right_y(pickup_height);
		const float x = from_left
			? -pickup_width - 20.0f
			: static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) + 20.0f;
		const float speed = from_left ? 220.0f : -220.0f;

		pickup->transform.position = glm::vec2(x, y);
		pickup->transform.velocity = glm::vec2(speed, 0.0f);
		pickups.push_back(std::move(pickup));
		return true;
	}

	void generate_pickup() {
		const float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		const bool is_bad = (r < bad_pickup_chance);
		bool shark_flag = false;
		bool can_flag = false;
		bool nemo_flag = false;
		const char* texture_file = is_bad ? get_bad_pickup_texture(shark_flag, can_flag) : get_good_pickup_texture(nemo_flag);
		const bool piranha_flag = is_bad && !shark_flag && !can_flag;

		auto pickup = std::make_unique<GameObject>(
			shark_flag ? "shark" : (piranha_flag ? "piranha" : (is_bad ? "bad pickup" : "good pickup")),
			glm::vec2(0.0f, 0.0f),
			0.0f,
			glm::vec2(1.0f),
			glm::vec2(0.0f)
		);
		pickup->set_renderer(
			std::make_unique<SpriteRenderer>(texture_file, 1, true)
		);

		auto* pickup_renderer = dynamic_cast<SpriteRenderer*>(pickup->get_renderer());
		const float pickup_height = (pickup_renderer != nullptr)
			? pickup_renderer->get_texture_height() * pickup->transform.scale.y
			: 0.0f;

		const AABB temp_bb = get_pickup_bounding_box(pickup.get());
		const float pickup_width = temp_bb.max_x - temp_bb.min_x;

		if (spawn_side(pickup, shark_flag, true, pickup_height, pickup_width)) return;
		if (spawn_side(pickup, piranha_flag || nemo_flag, false, pickup_height, pickup_width)) return;

		const float window_width = glutGet(GLUT_WINDOW_WIDTH);
		const float right_limit = std::max(0.0f, window_width - pickup_width);

		const AABB player_bb = get_player_bounding_box();
		const AABB safe_zone = expand_aabb(player_bb, GameConstants::SPAWN_MARGIN);

		bool placed = false;
		for (int i = 0; i < GameConstants::MAX_SPAWN_ATTEMPTS; ++i) {
			const float y = random_pickup_y(pickup_height);
			const float x = static_cast<float>(std::rand() % (static_cast<int>(right_limit) + 1));

			if (is_position_valid(glm::vec2(x, y), pickup.get(), safe_zone)) {
				placed = true;
				break;
			}
		}

		if (!placed) {
			const float x_fallback = glm::clamp(safe_zone.max_x + 10.0f, 0.0f, right_limit);
			const float y_fallback = random_pickup_y(pickup_height);
			pickup->transform.position = { x_fallback, y_fallback };
		}

		pickups.push_back(std::move(pickup));
	}

	bool is_pickup_out_of_screen(GameObject* pickup) {
		auto* renderer = dynamic_cast<SpriteRenderer*>(pickup->get_renderer());
		if (!renderer) return false;

		const float window_width = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
		const float pickup_width = renderer->get_texture_width() * pickup->transform.scale.x;

		const bool off_right = pickup->transform.position.x > (window_width + pickup_width);
		const bool off_left = pickup->transform.position.x < -pickup_width;

		return off_right || off_left;
	}

	bool check_for_collision(GameObject* pickup) {
		const AABB player_aabb = get_player_bounding_box();
		const AABB pickup_aabb = get_pickup_bounding_box(pickup);

		if (intersects(player_aabb, pickup_aabb)) {
			const bool is_shark = (std::strcmp(pickup->get_name(), "shark") == 0);
			if (is_shark) {
				shark_hit = true;
				return true;
			}

			const bool is_bad = (std::strcmp(pickup->get_name(), "bad pickup") == 0)
				|| (std::strcmp(pickup->get_name(), "piranha") == 0);
			score += is_bad ? -GameConstants::PICKUP_VALUE : GameConstants::PICKUP_VALUE;
			SoundManager::get_instance().play_pickup(is_bad);
			return true;
		}

		return false;
	}

	void restart_game() {
		auto it = scenes.find(this_scene_id);
		if (it != scenes.end()) {
			bool was_active = it->second.active;
			it->second.ptr = std::make_unique<GameScene>(bad_pickup_chance, this_scene_id);
			it->second.active = was_active;
			if (was_active) current_scene = it->second.ptr.get();
		}
	}

	void game_over() {
		SoundManager::get_instance().play_game_over();

		auto it = scenes.find(SceneId::game_over);
		if (it != scenes.end()) {
			it->second.ptr = std::make_unique<EndGameScene>(score, this_scene_id);
			it->second.active = false;
		}

		switch_scene(SceneId::game_over);
		restart_game();
	}

	void level_passed() {
		SoundManager::get_instance().play_passed_level();

		level1_score = score;

		auto it = scenes.find(SceneId::passed_the_level);
		if (it != scenes.end()) {
			it->second.ptr = std::make_unique<PassedTheLevelScene>(score);
			it->second.active = false;
		}

		switch_scene(SceneId::passed_the_level);
		restart_game();
	}

	void game_passed() {
		SoundManager::get_instance().play_passed_level();

		auto it = scenes.find(SceneId::passed_the_game);
		if (it != scenes.end()) {
			it->second.ptr = std::make_unique<PassedTheGameScene>(level1_score + score);
			it->second.active = false;
		}

		switch_scene(SceneId::passed_the_game);
		restart_game();
	}
};