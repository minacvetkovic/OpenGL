#pragma once
#include "GameObject.h"
#include "Sprite.h"
struct AABB {
	float min_x = 0, min_y = 0, max_x = 0, max_y = 0;
};

static inline AABB expand_aabb(const AABB& b, float m) {
	return { b.min_x - m, b.min_y - m, b.max_x + m, b.max_y + m };
}

static inline AABB make_aabb(GameObject* obj, bool use_frame_width, AABB offsets) {

	auto* renderer = dynamic_cast<SpriteRenderer*>(obj->get_renderer());

	const float width = (use_frame_width ? renderer->get_frame_width() : renderer->get_texture_width()) * obj->transform.scale.x;
	const float height = renderer->get_texture_height() * obj->transform.scale.y;

	const float x = obj->transform.position.x;
	const float y = obj->transform.position.y;

	AABB box;
	box.min_x = x + offsets.min_x;
	box.min_y = y + offsets.min_y;
	box.max_x = x + width + offsets.max_x;
	box.max_y = y + height + offsets.max_y;
	return box;
}

static inline bool intersects(const AABB& a, const AABB& b) {
	return (a.max_x > b.min_x && a.min_x < b.max_x && a.max_y > b.min_y && a.min_y < b.max_y);
}

static inline bool point_in_aabb(const AABB& a, float x, float y) {
	return x >= a.min_x && x <= a.max_x && y >= a.min_y && y <= a.max_y;
}

static inline void draw_aabb(const AABB& b, float r, float g, float bl) {
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(r, g, bl);
	glBegin(GL_LINE_LOOP);
	glVertex2f(b.min_x, b.min_y);
	glVertex2f(b.max_x, b.min_y);
	glVertex2f(b.max_x, b.max_y);
	glVertex2f(b.min_x, b.max_y);
	glEnd();
}