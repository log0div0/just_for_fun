
#pragma once

#include <math/Vector.hpp>
#include <math/Transform.hpp>
#include <math/Quaternion.hpp>

#include "Window.hpp"
#include "Utils.hpp"

struct Camera {
	math::Vector3 pos;
	math::Quaternion rot;
	float aspect = 1.0f;

	Camera(Window& window);

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
	Window& window;
};
