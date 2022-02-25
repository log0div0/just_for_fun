
#pragma once

#include <math/Transform.hpp>
#include <math/Quaternion.hpp>

struct Camera {
	math::Vector3 pos;
	math::Quaternion rot = math::Quaternion::Identity;
	float speed = 1.0f;

	void MoveForward(float delta_time);
	void MoveBack(float delta_time);
	void MoveRight(float delta_time);
	void MoveLeft(float delta_time);
	void MoveUp(float delta_time);
	void MoveDown(float delta_time);
	void Move(float delta_time, const math::Vector3& dir);

	math::Vector3 GetRightDirection() const;
	math::Vector3 GetForwardDirection() const;
	math::Transform GetViewTransform() const;
	math::Transform GetProjectionTransform(float aspect) const;
};
