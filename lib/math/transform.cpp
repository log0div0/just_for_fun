#include <catch.hpp>
#include "transform.hpp"

using namespace math;

TEST_CASE("transform order") {
	Transform t1 = Matrix3::MakeRotation(90._deg, Vector3(.0f, .5f, 1.f).Normalize());
	Transform t2 = Vector3{1.0f, 2.0f, -28.0f};

	Point v{5.0f, 1.0f, 2.6f};

	Point a = t2(t1(v));
	Point b = t1(t2(v));

	Point A = (t2*t1)(v);
	Point B = (t1*t2)(v);

	REQUIRE(a == Nearly(A));
	REQUIRE(b == Nearly(B));

	REQUIRE(a != Nearly(B));
	REQUIRE(b != Nearly(A));
}

TEST_CASE("transform order 2") {
	Transform rotation = Matrix3::MakeRotation(90._deg, Vector3(.0f, .5f, 1.f).Normalize());
	Transform translation = Vector3{1.0f, 2.0f, -28.0f};

	Transform both;
	both.SetTransofmationMatrix(Matrix3::MakeRotation(90._deg, Vector3(.0f, .5f, 1.f).Normalize()));
	both.SetTranslationVector({1.0f, 2.0f, -28.0f});

	REQUIRE(both == Nearly(translation*rotation));
	REQUIRE(both != Nearly(rotation*translation));
}

TEST_CASE("transform plane") {
	Transform t;
	t.SetTransofmationMatrix(Matrix3::MakeRotation(-60.0_deg, {1.0f, .0f, .0f}));
	Vector3 translation = {1.0f, 5.0f, 28.0f};
	t.SetTranslationVector(translation);

	Plane plane = t(Plane({0.0f, 0.0f, 1.0f}, -10.0f));
	REQUIRE(plane.n == Nearly(Vector3{0.0f, 0.866025f, 0.5f}));
	REQUIRE(plane.d == Nearly(-10.0f - translation.ProjectOnto(plane.n).Magnitude()));

	REQUIRE(plane.GetDistanceTo(t(Point(200.0f, -555.0f, 10.0f))) == Nearly(0.0f));
	REQUIRE(plane.GetDistanceTo(t(Point(-123.0f, 678.0f, 310.0f))) == Nearly(300.0f));
}

TEST_CASE("transform plane 2") {
	Point p1 = Point(1.0f, 2.0f, 3.0f);
	Point p2 = Point(-4.0f, 0.0f, 10.0f);
	Point p3 = Point(123.0f, 321.0f, 111.0f);

	Transform t;
	t.SetTransofmationMatrix(Matrix3::MakeRotation(49.0_deg, Vector3(18.0f, -10.0f, 12.0f).Normalize()));
	t.SetTranslationVector({-10.0f, 5.0f, 82.0f});

	Plane plane1 = t(Plane(p1, p2, p3));
	Plane plane2 = Plane(t(p1), t(p2), t(p3));

	REQUIRE(plane1 == Nearly(plane2));
}
