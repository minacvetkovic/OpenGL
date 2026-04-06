#include "GameObject.h"
#include "Constants.h"
#include "GameScene.h"
#include "EndGameScene.h"
#include "MenuScene.h"
#include "OptionsScene.h"
#include "Sprite.h"
#include "PassedTheLevelScene.h"
#include "PassedTheGameScene.h"
#include <vector>
#include <string>

float delta_time;
int previous_time = 0;

std::unordered_map<SceneId, SceneEntry> scenes;
Scene* current_scene;

void initialize() {
	scenes.emplace(SceneId::menu, SceneEntry{ std::make_unique<MenuScene>(), true });
	scenes.emplace(SceneId::game, SceneEntry{ std::make_unique<GameScene>(0.50f, SceneId::game), false });
	scenes.emplace(SceneId::game_level2, SceneEntry{ std::make_unique<GameScene>(0.70f, SceneId::game_level2), false });
	scenes.emplace(SceneId::passed_the_level, SceneEntry{ std::make_unique<PassedTheLevelScene>(), false });
	scenes.emplace(SceneId::passed_the_game, SceneEntry{ std::make_unique<PassedTheGameScene>(), false });
	scenes.emplace(SceneId::credits, SceneEntry{ std::make_unique<OptionsScene>(), false });
	scenes.emplace(SceneId::game_over, SceneEntry{ std::make_unique<EndGameScene>(0, SceneId::game), false });

	for (auto& [id, entry] : scenes) {
		if (entry.active && entry.ptr) {
			current_scene = entry.ptr.get();
			break;
		}
	}
}

void update(float dt) {

	if (current_scene) {
		current_scene->update(dt);
	}

	SoundManager::get_instance().update();
	Input::update();
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	if (current_scene) {
		current_scene->render();
	}

	glutSwapBuffers();
}

void game_loop(void) {
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	delta_time = (float)(current_time - previous_time) / 1000;
	previous_time = current_time;

	update(delta_time);
	render();

	glutPostRedisplay();
}

void init_game(void) {
	glClearColor(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	SoundManager::get_instance().init_fmod();
	SoundManager::get_instance().init_audio();
}

void reshape(int width, int height) {
	if (height == 0) height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0.0, width, 0.0, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Ocean Explorer");

	init_game();
	initialize();

	srand(static_cast<unsigned int>(time(nullptr)));

	Input::init();
	glutDisplayFunc(game_loop);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}