
#include "Camera.hpp"

using namespace math::literals;

Camera::Camera(Window& window_): window(window_) {
#ifndef _GAMING_XBOX
	window.OnWindowResize([&] (int w, int h) {
		aspect = float(w) / h;
	});
#endif
	auto [w, h] = window.GetWindowSize();
	aspect = float(w) / h;
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
	// movement
	pos += GetForwardDirection() * window.CameraForwardSpeed() * delta_time;
	pos += GetRightDirection() * window.CameraRightSpeed() * delta_time;
	pos += math::Vector3{0.0f, 0.0f, 1.0f} * window.CameraUpSpeed() * delta_time;

	// rotation
	{
		auto[dx, dy] = window.CameraRotationSpeed();
		constexpr float scale = 0.015f;
		math::Quaternion pitch = math::Quaternion::MakeRotation(dy * scale, GetRightDirection());
		math::Quaternion yaw = math::Quaternion::MakeRotation(dx * scale, {0.0f, 0.0f, 1.0f});
		rot = yaw * pitch * rot;
	}
}
