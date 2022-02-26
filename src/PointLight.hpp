
#pragma once

#include "Camera.hpp"

struct PointLight {
	void Update(float delta_time);
	void Render(const Camera& camera);
};
