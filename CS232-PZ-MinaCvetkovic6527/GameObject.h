#pragma once
#include <cmath>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include <glut.h>

#include "Transform2D.h"
#include "Primitive.h"
#include <memory>

class GameObject {
private:
	const char* name;
	std::unique_ptr<IRenderable> renderer;
public:
	Transform2D transform;

	virtual ~GameObject() = default;
	GameObject(const char* object_name, const glm::vec2& position, float rotation_deg, const glm::vec2& scale, const glm::vec2& velocity) {
		name = object_name;
		transform.position = position;
		transform.rotation_deg = rotation_deg;
		transform.scale = scale;
		transform.velocity = velocity;
	}

	void set_renderer(std::unique_ptr<IRenderable> r) { renderer = std::move(r); }
	IRenderable* get_renderer() const { return renderer.get(); }

	const char* get_name() const { return name; }

	void update(float delta_time) {
		transform.position += transform.velocity * delta_time;
		renderer->update(delta_time);
	}

	virtual void render() {
		if (!renderer) return;

		glPushMatrix();
		apply_transform(transform);
		renderer->render();
		glPopMatrix();
	}
};

