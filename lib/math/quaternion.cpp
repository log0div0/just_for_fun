#include <catch.hpp>
#include "quaternion.hpp"

using namespace math;

TEST_CASE("quaternion multiplication") {
	float x1 = 1.0f;
	float y1 = 2.0f;
	float z1 = 3.0f;
	float w1 = 4.0f;
	float x2 = 5.0f;
	float y2 = 6.0f;
	float z2 = 7.0f;
	float w2 = 8.0f;

	Quaternion q1(x1, y1, z1, w1);
	Quaternion q2(x2, y2, z2, w2);
	Quaternion m = q1*q2;
	Quaternion expected(
		x1*w2 + y1*z2 - z1*y2 + w1*x2,
		y1*w2 + z1*x2 + w1*y2 - x1*z2,
		z1*w2 + w1*z2 + x1*y2 - y1*x2,
		w1*w2 - x1*x2 - y1*y2 - z1*z2
	);
	REQUIRE(m == Nearly(expected));
}

TEST_CASE("quaternion magnitude") {
	Quaternion q1(1.2f, -1.4f, 8.1f, 1.44f);
	Quaternion q2(10.1f, 3.2f, -1.1f, 5.5f);
	Quaternion m = q1*q2;
	REQUIRE(m.Magnitude() == Nearly(q1.Magnitude() * q2.Magnitude()));
}

TEST_CASE("quaternion inverse") {
	Quaternion q(1.2f, -1.4f, 8.1f, 1.44f);
	Quaternion one(0.0f, 0.0f, 0.0f, 1.0f);
	REQUIRE(q * q.Inverse() == Nearly(one));
}

TEST_CASE("quaternion rotate vector") {
	Vector3 a = {15.f, 8.f, -5.f};
	Quaternion q = Quaternion::MakeRotation(90._deg, {.0f, .0f, 1.f});
	Vector3 res = q.Rotate(a);
	REQUIRE(res == Nearly(Vector3{-8.f, 15.f, -5.f}));
}

TEST_CASE("quaternion rotate vector 2") {
	Vector3 a = {10.f, 3.f, 0.f};
	Quaternion q = Quaternion::MakeRotation(-117.664_deg, {.0f, .0f, 1.f});
	Vector3 res = q.Rotate(a);
	REQUIRE(res == Nearly(Vector3{-1.98578f, -10.2497f, 0.f}));
}

TEST_CASE("quaternion rotate vector 3") {
	Vector3 a = {15.f, 8.f, -5.f};
	Quaternion q(0.0f, 0.0f, 0.0f, 1.0f);
	Vector3 res = q.Rotate(a);
	REQUIRE(res == Nearly(a));
}
