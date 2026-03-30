#pragma once
#include "Scene.h"
#include "Constants.h"
#include <string>
#include "EndGameScene.h"

class GameScene : public Scene {

private:
	std::unique_ptr<GameObject> background;
	std::unique_ptr<GameObject> player;

	std::vector<std::unique_ptr<GameObject>> pickups;

	int score = 0;
	float pickup_timer = 0.0f;
	float level_timer = 0.0f;
	bool is_paused = false;

	UIButton pause_button;
	UIButton exit_button;
public:
	explicit GameScene() {
		initialize();
	}

	void update(float delta_time) override {

		if (is_button_clicked(exit_button.box)) {
			switch_scene(SceneId::menu);
			restart_game();
			return;
		}

		check_pause();

		if (!is_paused) {

			check_background();
			move_player();
			check_edges_for_player();

			level_timer += delta_time;
			if (level_timer >= GameConstants::LEVEL_THRESHOLD) {
				game_over();
				return;
			}


			background->update(delta_time);
			player->update(delta_time);

			if (should_generate_pickups()) {
				pickup_timer += delta_time;
				if (pickup_timer >= GameConstants::PICKUPS_SPAWN_INTERVAL) {
					generate_pickup();
					pickup_timer = 0.0f;
				}
			}

			for (auto it = pickups.begin(); it != pickups.end();) {
				if (check_for_collision(it->get()))
					it = pickups.erase(it);
				else {
					(*it)->update(delta_time);
					++it;
				}
			}
		}
	}

	void render() override {
		background->render();

		for (const auto& pickup : pickups) {
			pickup->render();
			const AABB pickup_bb = get_pickup_bounding_box(pickup.get());
			draw_aabb(pickup_bb, 0.0f, 1.0f, 0.0f);
		}

		player->render();

		pause_button.button->render();
		exit_button.button->render();

		const AABB player_bb = get_player_bounding_box();
		draw_aabb(player_bb, 1.0f, 0.0f, 0.0f);

		const std::string score_text = "Score: " + std::to_string(score);
		draw_text(score_text.c_str(), 320, 560);
	}

private:
	void initialize() override {
		background = std::make_unique<GameObject>(
			/*name*/ "background",
			/*position*/ glm::vec2(0.0f, 0.0f),
			/*rotation_deg*/ 0.0f,
			/*scale*/ glm::vec2(0.4f),
			/*velocity*/ glm::vec2(-50.0f, 0.0f)
		);
		background->set_renderer(
			std::make_unique<SpriteRenderer>(
				/*file_name*/ "Sprites/background.png",
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

		auto pause_obj = std::make_unique<GameObject>(
			"pause_button",
			glm::vec2(50.f, 550.f),
			0.f,
			glm::vec2(1.f),
			glm::vec2(0.f)
		);

		pause_obj->set_renderer(std::make_unique<SpriteRenderer>(
			"Sprites/pauseButton.png",
			1,
			false));

		AABB pause_box = make_aabb(
			pause_obj.get(),
			false,
			/*offsets*/{ 0,0,0,0 });

		pause_button = {
			/*button*/ std::move(pause_obj),
			/*box*/ pause_box,
			/*active*/ true
		};

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
		return make_aabb(player.get(), true, get_player_offset());
	}

	AABB get_pickup_bounding_box(GameObject* pickup) {
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

	float get_random_pickup_y_position() {
		return (std::rand() % 2 == 0)
			? GameConstants::PICKUP_LOWER_POSITION
			: GameConstants::PICKUP_UPPER_POSITION;
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

	void move_player() {
		if (Input::get_key_down('W')) {
			if (player->transform.position.y < GameConstants::PLAYER_UPPER_POSITION) {
				player->transform.position.y = GameConstants::PLAYER_UPPER_POSITION;
				SoundManager::get_instance().audio_manager->playSound(
					SoundManager::get_instance().jump_sound,
					nullptr,
					false,
					&SoundManager::get_instance().environment_sounds
				);
			}
		}
		else if (Input::get_key('S')) {
			if (player->transform.position.y >= GameConstants::PLAYER_UPPER_POSITION) {
				player->transform.position.y = GameConstants::PLAYER_LOWER_POSITION;
				SoundManager::get_instance().audio_manager->playSound(
					SoundManager::get_instance().jump_sound,
					nullptr,
					false,
					&SoundManager::get_instance().environment_sounds
				);
			}
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

	const char* get_pickup_texture(int type) {
		static const char* textures[] = {
			"Sprites/water.png",
			"Sprites/rock.png",
			"Sprites/oil.png",
			"Sprites/battery.png"
		};
		return textures[type];
	}

	void generate_pickup() {
		const int rand_index = std::rand() % 4;
		const bool is_bad = (rand_index == 0 || rand_index == 1);

		auto pickup = std::make_unique<GameObject>(
			is_bad ? "bad pickup" : "good pickup",
			glm::vec2(0.0f, 0.0f),
			0.0f,
			glm::vec2(1.0f),
			glm::vec2(0.0f)
		);
		pickup->set_renderer(
			std::make_unique<SpriteRenderer>(get_pickup_texture(rand_index), 1, true)
		);

		const float window_width = glutGet(GLUT_WINDOW_WIDTH);
		const AABB temp_bb = get_pickup_bounding_box(pickup.get());
		const float pickup_width = temp_bb.max_x - temp_bb.min_x;
		const float right_limit = std::max(0.0f, window_width - pickup_width);

		const AABB player_bb = get_player_bounding_box();
		const AABB safe_zone = expand_aabb(player_bb, GameConstants::SPAWN_MARGIN);

		bool placed = false;
		for (int i = 0; i < GameConstants::MAX_SPAWN_ATTEMPTS; ++i) {
			const float y = get_random_pickup_y_position();
			const float x = static_cast<float>(std::rand() % (static_cast<int>(right_limit) + 1));

			if (is_position_valid(glm::vec2(x, y), pickup.get(), safe_zone)) {
				placed = true;
				break;
			}
		}

		if (!placed) {
			const float x_fallback = glm::clamp(safe_zone.max_x + 10.0f, 0.0f, right_limit);
			const float y_fallback = get_random_pickup_y_position();
			pickup->transform.position = { x_fallback, y_fallback };
		}

		pickups.push_back(std::move(pickup));
	}

	bool check_for_collision(GameObject* pickup) {
		const AABB player_aabb = get_player_bounding_box();
		const AABB pickup_aabb = get_pickup_bounding_box(pickup);

		if (intersects(player_aabb, pickup_aabb)) {
			const bool is_bad = (std::strcmp(pickup->get_name(), "bad pickup") == 0);
			score += is_bad ? -GameConstants::PICKUP_VALUE : GameConstants::PICKUP_VALUE;
			SoundManager::get_instance().audio_manager->playSound(
				is_bad ? SoundManager::get_instance().bad_pickup_sound : SoundManager::get_instance().good_pickup_sound,
				nullptr,
				false,
				&SoundManager::get_instance().environment_sounds
			);
			return true;
		}

		return false;
	}

	void check_pause() {
		if (is_button_clicked(/*pause button*/ pause_button.box)) {
			is_paused = !is_paused;

			std::string file_name = (is_paused) ? "Sprites/resumeButton.png" : "Sprites/pauseButton.png";
			pause_button.button->set_renderer(
				std::make_unique<SpriteRenderer>(/*file_name*/ file_name.c_str(), /*number_of_frames*/ 1, /*use_transparency*/ false)
			);
		}
	}

	void restart_game() {
		auto it = scenes.find(SceneId::game);
		if (it != scenes.end()) {
			bool was_active = it->second.active;
			it->second.ptr = std::make_unique<GameScene>();
			it->second.active = was_active;
			if (was_active) current_scene = it->second.ptr.get();
		}
	}

	void game_over() {
		auto it = scenes.find(SceneId::game_over);
		if (it != scenes.end()) {
			it->second.ptr = std::make_unique<EndGameScene>(score);
			it->second.active = false;
		}

		switch_scene(SceneId::game_over);
		restart_game();
	}
};