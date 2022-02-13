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
	Matrix3 ident = Matrix3::MakeIdentity();
	REQUIRE(c == Nearly(ident));
}

TEST_CASE("MakeProjectionOnto") {
	Vector3 a {10.3f, -5.4f, 12.1f};
	Vector3 b {8.4f, 12.0f, -2.3f};
	Matrix3 m = Matrix3::MakeProjectionOnto(b);
	Vector3 expected = a.ProjectOnto(b);
	REQUIRE(m*a == Nearly(expected));
}

TEST_CASE("MakeCross") {
	Vector3 a {10.3f, -5.4f, 12.1f};
	Vector3 b {8.4f, 12.0f, -2.3f};
	Matrix3 m = Matrix3::MakeCross(a);
	Vector3 expected = a.Cross(b);
	REQUIRE(m*b == Nearly(expected));
}

TEST_CASE("MakeRotation") {
	Vector3 a = {15.f, 8.f, -5.f};
	Matrix3 m = Matrix3::MakeRotation(90._deg, {.0f, .0f, 1.f});
	Vector3 res = m*a;
	REQUIRE(res == Nearly(Vector3{-8.f, 15.f, -5.f}));
	REQUIRE_ORTHOGONAL(m);
	REQUIRE(m.Determinant() == Nearly(1.0f));
	// columns of m are axes of the target coordinate system
	Matrix3 m2 {
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};
	REQUIRE(m == Nearly(m2));
}

TEST_CASE("MakeRotation 2") {
	Vector3 a = {10.f, 3.f, 0.f};
	Matrix3 m = Matrix3::MakeRotation(-117.664_deg, {.0f, .0f, 1.f});
	Vector3 res = m*a;
	REQUIRE(res == Nearly(Vector3{-1.98578f, -10.2497f, 0.f}));
	REQUIRE_ORTHOGONAL(m);
	REQUIRE(m.Determinant() == Nearly(1.0f));
}

TEST_CASE("MakeReflection") {
	Vector3 a = {1.f, 2.f, 3.f};
	Matrix3 m = Matrix3::MakeReflection({.0f, .0f, 1.f});
	Vector3 res = m*a;
	REQUIRE(res == Nearly(Vector3{1.f, 2.f, -3.f}));
	REQUIRE_ORTHOGONAL(m);
	REQUIRE(m.Determinant() == Nearly(-1.0f));
}

TEST_CASE("MakeInvolution") {
	Vector3 a = {1.f, 2.f, 3.f};
	Matrix3 m = Matrix3::MakeInvolution({.0f, .0f, 1.f});
	Vector3 res = m*a;
	REQUIRE(res == Nearly(Vector3{-1.f, -2.f, 3.f}));
	REQUIRE_ORTHOGONAL(m);
	REQUIRE(m.Determinant() == Nearly(1.0f));
}

TEST_CASE("MakeScale") {
	Vector3 a = {1.f, 2.f, 3.f};
	Matrix3 m = Matrix3::MakeScale(4.f);
	Vector3 res = m*a;
	REQUIRE(res == Nearly(Vector3{4.f, 8.f, 12.f}));
	REQUIRE(m.Determinant() == Nearly(64.0f));
}

TEST_CASE("MakeScale2") {
	Vector3 a = {1.f, 2.f, 3.f};
	Matrix3 m = Matrix3::MakeScale(4.f, {0.0f, 1.0f, 0.0f});
	Vector3 res = m*a;
	REQUIRE(res == Nearly(Vector3{1.f, 8.f, 3.f}));
	REQUIRE(m.Determinant() == Nearly(4.0f));
}

// s⨯t=n
// Ms⨯Mt = (Sx*M0+Sy*M1+Sz*M2)⨯(Tx*M0+Ty*M1+Tz*M2) =
//
// = SxTxM0⨯M0 + SxTyM0⨯M1 + SxTzM0⨯M2
// + SyTxM1⨯M0 + SyTyM1⨯M1 + SyTzM1⨯M2
// + SzTxM2⨯M0 + SzTyM2⨯M1 + SzTzM2⨯M2 =
//
// = 0         + SxTyM0⨯M1 - SxTzM2⨯M0
// - SyTxM0⨯M1 + 0         + SyTzM1⨯M2
// + SzTxM2⨯M0 - SzTyM1⨯M2 + 0         =
//
// = (SyTz-SzTy)M1⨯M2
// + (SzTx-SxTz)M2⨯M0
// + (SxTy-SyTx)M0⨯M1 =
//
// = Cofactor(M)*(s⨯t) = (s⨯t)*Adjugate(M)
TEST_CASE("transform antivector") {
	Matrix3 m = Matrix3::MakeScale(4.0f, {0.4f, 3.0f, -0.2f});

	Vector3 s = {1.0f, 2.0f, 3.0f};
	Vector3 t = {9.0f, -1.0f, 4.5f};

	Vector3 n1 = (m*s).Cross(m*t);
	Vector3 n2 = m.Cofactor()*s.Cross(t);

	REQUIRE(n1 == Nearly(n2));
}
