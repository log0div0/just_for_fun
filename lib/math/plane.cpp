#include <catch.hpp>
#include "plane.hpp"

using namespace math;

TEST_CASE("distance between plane and point") {
	Plane plane{ {2.0f, 4.0f, 3.0f}, 5.0f };
	Vector3 point {1.0f, 2.0f, 3.0f};
	float d = plane.GetDistanceTo(point);
	REQUIRE(d == Nearly(4.4566881f));
}

TEST_CASE("intersection plane and line") {
	// https://www.geogebra.org/calculator/cv5xy9jm

	Plane plane { {2.0f, -1.0f, 3.0f}, -15.0f };
	Line line {
		{4.0f, -1.0f, 3.0f},
		{1.0f, 8.0f, -2.0f}
	};
	std::optional<Vector3> intersection = plane.GetIntersectionWith(line);
	REQUIRE(intersection.has_value());
	REQUIRE(intersection.value() == Nearly(Vector3{4.25f, 1.0f, 2.5f}));
}

TEST_CASE("intersection plane and line parallel") {
	Plane plane { {-3.0f, -2.0f, 1.0f}, -4.0f };
	Line line {
		{2.0f, -3.0f, 4.0f},
		{2.0f, -4.0f, -2.0f}
	};
	std::optional<Vector3> intersection = plane.GetIntersectionWith(line);
	REQUIRE(!intersection.has_value());
}
