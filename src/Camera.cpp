
#include "Camera.hpp"

using namespace math::literals;

void Camera::MoveForward(float delta_time) {
	Move(delta_time, GetForwardDirection());
}

void Camera::MoveBack(float delta_time) {
	Move(delta_time, -GetForwardDirection());
}

void Camera::MoveRight(float delta_time) {
	Move(delta_time, GetRightDirection());
}

void Camera::MoveLeft(float delta_time) {
	Move(delta_time, -GetRightDirection());
}

void Camera::MoveUp(float delta_time) {
	Move(delta_time, {0.0f, 0.0f, 1.0f});
}

void Camera::MoveDown(float delta_time) {
	Move(delta_time, {0.0f, 0.0f, -1.0f});
}

void Camera::Move(float delta_time, const math::Vector3& dir) {
	pos += dir * delta_time * speed;
}

void Camera::OnCursorMove(float dx, float dy) {
	constexpr float scale = 0.005f;
	math::Quaternion pitch = math::Quaternion::MakeRotation(dy * scale, GetRightDirection());
	math::Quaternion yaw = math::Quaternion::MakeRotation(dx * scale, {0.0f, 0.0f, 1.0f});
	rot = yaw * pitch * rot;
}

void Camera::OnScroll(float diff) {
	constexpr float min_speed = 0.5f;
	constexpr float speed_step = 0.5f;
	speed += speed_step * diff;
	if (speed < min_speed) {
		speed = min_speed;
	}
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
	t.m.At(1,1) *= -1;
	return t;
}
