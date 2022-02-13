#include <catch.hpp>
#include "line.hpp"

using namespace math;

TEST_CASE("distance between line and point") {
	Line line{
		{5.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 0.0f}
	};

	Vector3 point {17.0f, 8.0f, 0.0f};
	float d = line.GetDistanceTo(point);
	REQUIRE(d == Nearly(2.82842712f));
}

TEST_CASE("distance between two lines") {
	Line line1{
		{-1.0f, 2.0f, 0.0f},
		{2.0f, 3.0f, 1.0f}
	};
	Line line2{
		{3.0f, -4.0f, 1.0f},
		{1.0f, 2.0f, 1.0f}
	};

	{
		float d = line1.GetDistanceTo(line2);
		REQUIRE(d == Nearly(6.3508529610859f));
	}
	{
		float d = line2.GetDistanceTo(line1);
		REQUIRE(d == Nearly(6.3508529610859f));
	}
}

TEST_CASE("distance between two parallel lines") {
	Line line1{
		{-1.0f, 2.0f, 0.0f},
		{2.0f, 3.0f, 0.0f}
	};
	Line line2{
		{-1.0f, 2.0f, 10.0f},
		{2.0f, 3.0f, 0.0f}
	};

	float d = line1.GetDistanceTo(line2);
	REQUIRE(d == Nearly(10.0f));
}
