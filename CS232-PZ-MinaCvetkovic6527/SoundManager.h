#pragma once
#include "fmod.hpp"

class SoundManager {
private:
	void release() {
		if (background_sound) background_sound->release();
		if (good_pickup_sound) good_pickup_sound->release();
		if (bad_pickup_sound) bad_pickup_sound->release();
		if (jump_sound) jump_sound->release();
		if (button_sound) button_sound->release();

		if (audio_manager) {
			audio_manager->close();
			audio_manager->release();
			audio_manager = nullptr;
		}
	}

	bool create_sound(const char* file_path, FMOD_MODE mode, FMOD::Sound** sound) {
		if (!audio_manager) return false;

		const FMOD_RESULT result = audio_manager->createSound(file_path, mode, nullptr, sound);
		if (result != FMOD_OK) {
			*sound = nullptr;
			return false;
		}

		return true;
	}

public:
	static SoundManager& get_instance() {
		static SoundManager instance;
		return instance;
	}

	~SoundManager() { release(); }

	FMOD::System* audio_manager = nullptr;
	FMOD::Channel* surrounding_sounds = nullptr;
	FMOD::Channel* environment_sounds = nullptr;

	FMOD::Sound* background_sound = nullptr;
	FMOD::Sound* good_pickup_sound = nullptr;
	FMOD::Sound* bad_pickup_sound = nullptr;
	FMOD::Sound* jump_sound = nullptr;
	FMOD::Sound* button_sound = nullptr;

	bool init_fmod() {
		FMOD_RESULT result = FMOD::System_Create(&audio_manager);
		if (result != FMOD_OK) return false;

		result = audio_manager->init(50, FMOD_INIT_NORMAL, nullptr);
		if (result != FMOD_OK) return false;

		return true;
	}

	bool init_audio() {
		if (!create_sound("Sounds/Background.mp3", FMOD_2D | FMOD_LOOP_NORMAL, &background_sound)) return false;
		if (!create_sound("Sounds/button.mp3", FMOD_DEFAULT, &button_sound)) return false;
		if (!create_sound("Sounds/moving.mp3", FMOD_DEFAULT, &jump_sound)) return false;
		if (!create_sound("Sounds/AddPoint.mp3", FMOD_DEFAULT, &good_pickup_sound)) return false;
		if (!create_sound("Sounds/LosePoint.mp3", FMOD_DEFAULT, &bad_pickup_sound)) return false;

		FMOD_RESULT result = audio_manager->playSound(background_sound, nullptr, false, &surrounding_sounds);
		if (result != FMOD_OK) return false;

		return true;
	}

	void update() {
		audio_manager->update();
	}
};