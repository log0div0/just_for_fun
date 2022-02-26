
#pragma once

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include <glfwpp/glfwpp.h>

#include <math/Vector.hpp>
#include <math/Transform.hpp>
#include <math/Quaternion.hpp>

struct Camera {
	math::Vector3 pos = math::Vector3::Zero;
	math::Quaternion rot = math::Quaternion::Identity;
	float speed = 1.0f;
	float aspect = 1.0f;

	Camera(glfw::Window& window);

	Camera(const Camera& other) = delete;
	Camera(Camera&& other) = delete;

	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other) = delete;

	math::Vector3 GetRightDirection() const;
	math::Vector3 GetForwardDirection() const;
	math::Transform GetViewTransform() const;
	math::Transform GetProjectionTransform() const;

	void Update(float delta_time);

private:
	void Move(float delta_time, const math::Vector3& dir);
	void OnCursorMove(float dx, float dy);
	void OnScroll(float diff);

	glfw::Window& window;

	using CursorPos = math::Vector<double, 2>;
	CursorPos last_cursor_pos;
};
