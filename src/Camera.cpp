
#include "Camera.hpp"

using namespace math::literals;

Camera::Camera(Window& window_): window(window_) {
	window.OnCameraRotate([&] (float dx, float dy) {
		constexpr float scale = 0.005f;
		math::Quaternion pitch = math::Quaternion::MakeRotation(dy * scale, GetRightDirection());
		math::Quaternion yaw = math::Quaternion::MakeRotation(dx * scale, {0.0f, 0.0f, 1.0f});
		rot = yaw * pitch * rot;
	});

	window.OnCameraAccelerate([&] (float diff) {
		constexpr float min_speed = 0.5f;
		constexpr float speed_step = 0.5f;
		speed += speed_step * diff;
		if (speed < min_speed) {
			speed = min_speed;
		}
	});

#ifndef _GAMING_XBOX
	window.OnWindowResize([&] (int w, int h) {
		aspect = float(w) / h;
	});
#endif
	auto [w, h] = window.GetWindowSize();
	aspect = float(w) / h;
}

void Camera::Move(float delta_time, const math::Vector3& dir) {
	pos += dir * delta_time * speed;
}

math::Vector3 Camera::GetForwardDirection() const {
	math::Vector3 initial_dir = {-1.0f, 0.0f, 0.0f};
	math::Vector3 dir = rot.Rotate(initial_dir);
	return dir;
}

math::Vector3 Camera::GetRightDirection() const {
	return GetForwardDirection().Cross({0.0f, 0.0f, 1.0f});
}

math::Transform Camera::GetViewTransform() const {
	return math::LookAt(pos, pos + GetForwardDirection(), {0.0f, 0.0f, 1.0f});
}

math::Transform Camera::GetProjectionTransform() const {
	math::Transform t = math::Perspective(60_deg, aspect, 0.1f, 1000.0f);
	t.At(1,1) *= -1;
	return t;
}

void Camera::Update(float delta_time) {
	if (window.IsCameraMovingForward()) {
		Move(delta_time, GetForwardDirection());
	}
	if (window.IsCameraMovingBack()) {
		Move(delta_time, -GetForwardDirection());
	}
	if (window.IsCameraMovingRight()) {
		Move(delta_time, GetRightDirection());
	}
	if (window.IsCameraMovingLeft()) {
		Move(delta_time, -GetRightDirection());
	}
	if (window.IsCameraMovingUp()) {
		Move(delta_time, {0.0f, 0.0f, 1.0f});
	}
	if (window.IsCameraMovingDown()) {
		Move(delta_time, {0.0f, 0.0f, -1.0f});
	}
}
