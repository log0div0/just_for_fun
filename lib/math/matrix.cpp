#include <catch.hpp>
#include "matrix.hpp"

using namespace math;

TEST_CASE("matrix * matrix") {
	Matrix<int, 2, 3> a{
		1, 3, -2,
		0, -1, 4
	};
	Matrix<int, 3, 2> b{
		2, -2,
		1, 5,
		-3, 4
	};
	Matrix<int, 2, 2> c{
		11, 5,
		-13, 11
	};
	REQUIRE((a * b) == c);
}

TEST_CASE("matrix * vector") {
	Matrix<int, 2, 3> a{
		1, 3, -2,
		0, -1, 4
	};
	Vector<int, 3> b{
		10,
		-10,
		2
	};
	Vector<int, 2> c{
		-24,
		18
	};
	REQUIRE((a * b) == c);
}

TEST_CASE("matrix * vector 2") {
	Matrix4 m {
		1.0f, 0.0f, 0.0f, -0.5f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	Vector4 v = {-0.5f, -0.5f, 0.0f, 1.0f};
	Vector4 res = m*v;
	REQUIRE(res == Nearly(Vector4{-1.0f, -0.5f, 0.0f, 1.0f}));
}

TEST_CASE("Product rule for matrix transpose") {
	Matrix<int, 2, 3> a{
		1, 3, -2,
		0, -1, 4
	};
	Matrix<int, 3, 2> b{
		2, -2,
		1, 5,
		-3, 4
	};
	REQUIRE((a*b).Transpose() == (b.Transpose() * a.Transpose()));
}


TEST_CASE("Matrix determinant") {
	Matrix3 a {
		1.5f, 20.1f, 10.8f,
		-10.4f, 4.1f, -1.1f,
		8.4f, 12.0f, -2.3f
	};
	float det = a.Determinant();
	REQUIRE(det == Nearly(-2380.65283f));
}

TEST_CASE("Matrix inverse") {
	Matrix3 a {
		1.5f, 20.1f, 10.8f,
		-10.4f, 4.1f, -1.1f,
		8.4f, 12.0f, -2.3f
	};
	Matrix3 b = a.Inverse();
	Matrix3 c = a * b;
	REQUIRE(c == Nearly(Matrix3::Identity()));
}
