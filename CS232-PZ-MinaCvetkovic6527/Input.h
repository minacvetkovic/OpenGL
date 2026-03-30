#pragma once
#include <unordered_map>
#include "glut.h"
#include "glm.hpp"

class Input {
private:
	Input() = delete;

private:
	static std::unordered_map<int, bool> key_states;
	static std::unordered_map<int, bool> last_key_states;

	static std::unordered_map<int, bool> special_states;
	static std::unordered_map<int, bool> last_special_states;

	static std::unordered_map<int, bool> mouse_button_states;
	static std::unordered_map<int, bool> last_mouse_button_states;

	static glm::vec2 mouse_position;
	static glm::vec2 last_mouse_position;
	static glm::vec2 delta_mouse_position;

	static bool is_cursor_locked;

	static int normalize_key(int key) {
		if (isalpha(static_cast<unsigned char>(key)))
			return toupper(static_cast<unsigned char>(key));
		return key;
	}

public:
	//held
	static bool get_key(int key) {
		auto it = key_states.find(normalize_key(key));
		return (it != key_states.end()) ? it->second : false;
	}
	//frame pressed
	static bool get_key_down(int key) {
		return get_key(key) && !last_key_states[normalize_key(key)];
	}
	//frame released
	static bool get_key_up(int key) {
		return !get_key(key) && last_key_states[normalize_key(key)];
	}

	static bool get_special_key(int key) {
		auto it = special_states.find(key);
		return (it != special_states.end()) ? it->second : false;
	}

	static bool get_special_key_down(int key) {
		return get_special_key(key) && !last_special_states[key];
	}

	static bool get_special_key_up(int key) {
		return !get_special_key(key) && last_special_states[key];
	}

	static bool get_mouse_button(int button) {
		auto it = mouse_button_states.find(button);
		return (it != mouse_button_states.end()) ? it->second : false;
	}

	static bool get_mouse_button_down(int button) {
		return get_mouse_button(button) && !last_mouse_button_states[button];
	}

	static bool get_mouse_button_up(int button) {
		return !get_mouse_button(button) && last_mouse_button_states[button];
	}

	static void set_cursor_lock(const bool& lock) {
		is_cursor_locked = lock;

		if (is_cursor_locked) {
			GLfloat width = glutGet(GLUT_WINDOW_WIDTH);
			GLfloat height = glutGet(GLUT_WINDOW_HEIGHT);

			glutSetCursor(GLUT_CURSOR_NONE);
			glutWarpPointer((int)width / 2, (int)height / 2);
		}
		else {
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
	}

	static void get_mouse_position(float* x, float* y) {
		*x = delta_mouse_position.x;
		*y = delta_mouse_position.y;
	}

	inline static void get_mouse_screen_position(float* x, float* y) {
		*x = mouse_position.x;
		*y = mouse_position.y;
	}

public:
	static void key_down_callback(unsigned char key, int x, int y) {
		key_states[normalize_key(key)] = true;
	}

	static void key_up_callback(unsigned char key, int x, int y) {
		key_states[normalize_key(key)] = false;
	}

	static void special_down_callback(int key, int x, int y) {
		special_states[key] = true;
	}

	static void special_up_callback(int key, int x, int y) {
		special_states[key] = false;
	}

	static void mouse_button_callback(int button, int state, int x, int y) {
		if (state == GLUT_DOWN)
			mouse_button_states[button] = true;
		else if (state == GLUT_UP)
			mouse_button_states[button] = false;
	}

	static void mouse_motion_callback(int x, int y) {
		mouse_position.x = x;
		mouse_position.y = y;
	}

public:
	static void init() {
		glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
		glutKeyboardFunc(key_down_callback);
		glutKeyboardUpFunc(key_up_callback);
		glutSpecialFunc(special_down_callback);
		glutSpecialUpFunc(special_up_callback);
		glutMouseFunc(mouse_button_callback);
		glutPassiveMotionFunc(mouse_motion_callback);
	}

	static void update() {
		last_key_states.clear();
		last_key_states.insert(key_states.begin(), key_states.end());

		last_special_states.clear();
		last_special_states.insert(special_states.begin(), special_states.end());

		last_mouse_button_states.clear();
		last_mouse_button_states.insert(mouse_button_states.begin(), mouse_button_states.end());

		set_cursor_lock(is_cursor_locked);

		if (!is_cursor_locked) {
			delta_mouse_position = last_mouse_position - mouse_position;
			last_mouse_position = mouse_position;
		}
		else {
			delta_mouse_position.x = glutGet(GLUT_WINDOW_WIDTH) / 2 - mouse_position.x;
			delta_mouse_position.y = glutGet(GLUT_WINDOW_HEIGHT) / 2 - mouse_position.y;
		}
	}
};

std::unordered_map<int, bool> Input::key_states;
std::unordered_map<int, bool> Input::last_key_states;

std::unordered_map<int, bool> Input::special_states;
std::unordered_map<int, bool> Input::last_special_states;

std::unordered_map<int, bool> Input::mouse_button_states;
std::unordered_map<int, bool> Input::last_mouse_button_states;

glm::vec2 Input::last_mouse_position = glm::vec2(0);
glm::vec2 Input::mouse_position = glm::vec2(0);
glm::vec2 Input::delta_mouse_position = glm::vec2(0);

bool Input::is_cursor_locked = false;