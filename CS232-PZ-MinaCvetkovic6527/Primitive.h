#pragma once
#include <cmath>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <glut.h>

#include "IRenderable.h"

#define DEG2RAD 3.14159265359 / 180.0

class PrimitiveRenderer : public IRenderable {
protected:
	glm::vec3 line_color;
	glm::vec3 fill_color;
public:

	PrimitiveRenderer() : line_color(0.0), fill_color(1.0) {}
	PrimitiveRenderer(const glm::vec3& new_line_color, const glm::vec3& new_fill_color)
		: line_color(new_line_color), fill_color(new_fill_color) {
	}
	~PrimitiveRenderer() {}

	glm::vec3 get_line_color() const { return line_color; }
	void set_line_color(const glm::vec3& new_line_color) { line_color = new_line_color; }

	glm::vec3 get_fill_color() const { return fill_color; }
	void set_fill_color(const glm::vec3& new_fill_color) { fill_color = new_fill_color; }
};

class CircleRenderer : public PrimitiveRenderer {
private:
	float radius;
public:
	CircleRenderer() : PrimitiveRenderer(), radius(20.0f) {}
	CircleRenderer(float new_radius, const glm::vec3& line_color, const glm::vec3& fill_color)
		: PrimitiveRenderer(line_color, fill_color), radius(new_radius) {
	}
	~CircleRenderer() {}

	float get_radius() const { return radius; }
	void set_radius(const float new_radius) { radius = new_radius; }

	void render() override {

		glLineWidth(5.0f);
		glColor3f(line_color.r, line_color.g, line_color.b);
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < 360; i += 5) {
			float theta = i * DEG2RAD;
			glVertex2f(cos(theta) * radius, sin(theta) * radius);
		}
		glEnd();

		glColor3f(fill_color.r, fill_color.g, fill_color.b);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i += 5) {
			float theta = i * DEG2RAD;
			glVertex2f(cos(theta) * radius, sin(theta) * radius);
		}
		glEnd();
	}
	void update(float delta_time) override {}
};

class CubeRenderer : public PrimitiveRenderer {
private:
	float size;
public:
	CubeRenderer() : PrimitiveRenderer(), size(0) {}
	CubeRenderer(float new_size, const glm::vec3& line_color, const glm::vec3& fill_color)
		: PrimitiveRenderer(line_color, fill_color), size(new_size) {
	}
	~CubeRenderer() {}

	float get_size() const { return size; }
	void set_size(const float new_size) { size = new_size; }

	void render() override {
		glLineWidth(2.0f);
		glColor3f(line_color.r, line_color.g, line_color.b);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-size / 2, -size / 2);
		glVertex2f(size / 2, -size / 2);
		glVertex2f(size / 2, size / 2);
		glVertex2f(-size / 2, size / 2);
		glEnd();

		glColor3f(fill_color.r, fill_color.g, fill_color.b);
		glBegin(GL_POLYGON);
		glVertex2f(-size / 2, -size / 2);
		glVertex2f(size / 2, -size / 2);
		glVertex2f(size / 2, size / 2);
		glVertex2f(-size / 2, size / 2);
		glEnd();
	}
	void update(float delta_time) override {}
};

class TriangleRenderer : public PrimitiveRenderer {
private:
	float base, height;
public:
	TriangleRenderer() : PrimitiveRenderer(), base(0), height(0) {}
	TriangleRenderer(float new_base, float new_height, const glm::vec3& line_color, const glm::vec3& fill_color)
		: PrimitiveRenderer(line_color, fill_color), base(new_base), height(new_height) {
	}
	~TriangleRenderer() {}

	float get_base() const { return base; }
	void set_base(const float new_base) { base = new_base; }
	float get_height() const { return height; }
	void set_height(const float new_height) { height = new_height; }

	void render() override {
		glLineWidth(2.0f);
		glColor3f(line_color.r, line_color.g, line_color.b);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-base / 2, -height / 2);
		glVertex2f(base / 2, -height / 2);
		glVertex2f(0.0f, height / 2);
		glEnd();

		glColor3f(fill_color.r, fill_color.g, fill_color.b);
		glBegin(GL_POLYGON);
		glVertex2f(-base / 2, -height / 2);
		glVertex2f(base / 2, -height / 2);
		glVertex2f(0.0f, height / 2);
		glEnd();
	}
	void update(float delta_time) override {}
};