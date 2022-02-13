#include <catch.hpp>
#include "vector.hpp"

using namespace math;

TEST_CASE("vector sum") {
	Vector<int, 3> a{1, 2, 3};
	Vector<int, 3> b{4, 5, 6};
	Vector<int, 3> c = a + b;
	REQUIRE(c == Vector<int, 3>{5, 7, 9});
}

TEST_CASE("vector magnitude") {
	Vector<int, 2> a{100, 100};
	REQUIRE(a.Magnitude() == Nearly(141.42135f));
}

TEST_CASE("vector normalize") {
	Vector3 a{1.5f, 100.0f, -21.1f};
	REQUIRE(a.Normalize().Magnitude() == Nearly(1.0f));
}

TEST_CASE("vector cross") {
	Vector<int, 3> a{1, -8, 12};
	Vector<int, 3> b{4, 6, 3};
	Vector<int, 3> c{-96,  45,  38};
	REQUIRE(a.Cross(b) == c);
}

TEST_CASE("vector dot product magnitude") {
	float angle = 28.0_deg;
	Vector2 a{1.0f, 0.0f};
	Vector2 b{1.0f * std::cos(angle), 1.0f * std::sin(angle)};

	REQUIRE(a.Dot(b) == Nearly(a.Magnitude() * b.Magnitude() * std::cos(angle)));
}

TEST_CASE("vector cross product magnitude") {
	float angle = 28.0_deg;
	Vector3 a{1.0f, 0.0f, 0.0f};
	Vector3 b{1.0f * std::cos(angle), 1.0f * std::sin(angle), 0.0f};

	REQUIRE(a.Cross(b).Magnitude() == Nearly(a.Magnitude() * b.Magnitude() * std::sin(angle)));
}

TEST_CASE("vector projection") {
	Vector2 a{4.0f, 2.0f};
	Vector2 b{-3.0f, 5.0f};
	Vector2 c = a.ProjectOnto(b);
	REQUIRE(c == Nearly(Vector2{0.176470f, -0.294117f}));
}

TEST_CASE("rotate vector") {
	Vector3 a = {15.f, 8.f, -5.f};
	Vector3 res = a.Rotate(90._deg, {.0f, .0f, 1.f});
	REQUIRE(res == Nearly(Vector3{-8.f, 15.f, -5.f}));
}

TEST_CASE("rotate vector 2") {
	Vector3 a = {10.f, 3.f, 0.f};
	Vector3 res = a.Rotate(-117.664_deg, {.0f, .0f, 1.f});
	REQUIRE(res == Nearly(Vector3{-1.98578f, -10.2497f, 0.f}));
}
