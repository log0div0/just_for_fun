
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

math::Transform Camera::GetProjectionTransform(float aspect) const {
	return math::Perspective(60_deg, aspect, 0.1f, 1000.0f);
}