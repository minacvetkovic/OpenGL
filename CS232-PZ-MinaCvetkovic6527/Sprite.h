#pragma once
#include "IRenderable.h"
#include "SOIL2.h"
#include <iostream>

class SpriteRenderer : public IRenderable {
private:
	GLuint textures;
	int texture_width, texture_height;

	int current_frame = 0;
	int total_frames = 0;

	float animation_delay = 0.30f;
	float animation_elapsed_time = 0.0f;

	bool is_transparent;
	bool x_flip = false;

public:
	SpriteRenderer() = default;
	SpriteRenderer(const char* new_file_name, int number_of_frames, bool use_transparency = false) : total_frames(number_of_frames), is_transparent(use_transparency) {
		textures = load_texture(new_file_name, &texture_width, &texture_height);
		if (textures == 0) std::cout << "Texture loading failed: " << SOIL_last_result() << std::endl;
	}
	~SpriteRenderer() {
		if (textures != 0) glDeleteTextures(1, &textures);
	}

	int get_texture_width() const { return texture_width; }
	float get_frame_width() const { return ((float)texture_width / (float)total_frames); }
	int get_texture_height() const { return texture_height; }

	void update(float delta_time) override {
		if (total_frames > 1) {
			animation_elapsed_time += delta_time;
			if (animation_elapsed_time >= animation_delay) {
				current_frame = (current_frame + 1) % total_frames;
				animation_elapsed_time = 0.0f;
			}
		}
	}

	void horizontal_flip(bool should_flip) { x_flip = should_flip; }
	bool is_x_flipped() const { return x_flip; }

	void render() override {
		if (is_transparent) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glBindTexture(GL_TEXTURE_2D, textures);
		glColor3f(1.0f, 1.0f, 1.0f);

		GLfloat x = 0;
		GLfloat y = 0;

		GLfloat w = texture_width / total_frames;
		GLfloat h = texture_height;

		float inv_frames = 1.0f / static_cast<float>(total_frames);

		float u0 = current_frame * inv_frames;
		float u1 = u0 + inv_frames;

		float v0 = 0.0f;
		float v1 = 1.0f;

		if (x_flip) std::swap(u0, u1);


		glBegin(GL_QUADS);
		glTexCoord2f(u0, v1); glVertex2f(x, y);
		glTexCoord2f(u1, v1); glVertex2f(x + w, y);
		glTexCoord2f(u1, v0); glVertex2f(x + w, y + h);
		glTexCoord2f(u0, v0); glVertex2f(x, y + h);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);

		if (is_transparent) glDisable(GL_BLEND);
	}

private:
	GLuint load_texture(const char* file_path, int* width = nullptr, int* height = nullptr) {

		GLuint texture_id = SOIL_load_OGL_texture(
			file_path,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			0
		);

		if (texture_id == 0) return 0;

		if (width && height) unsigned char* image_data = SOIL_load_image(file_path, width, height, 0, SOIL_LOAD_AUTO);

		return texture_id;
	}
};