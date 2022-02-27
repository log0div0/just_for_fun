
#pragma once

#include "matrix.hpp"
#include "plane.hpp"
#include "quaternion.hpp"

namespace math {

// a general affine transformation from coordinate system A to coordinate system B:
// pᴮ = Mpᴬ + t
// where M is a 3x3 transformation matrix and t is a 3D translation vector
//
// Transformation matrix H has the form
//     ┌     ┐
// H = | M t |
//     | 0 1 |
//     └     ┘
// Reverse transformation has the form
// pᴬ = M⁻¹pᴮ - M⁻¹t
// so
//       ┌           ┐
// H⁻¹ = | M⁻¹ -M⁻¹t |
//       | 0    1    |
//       └           ┘
struct Transform: Matrix4 {
	static Matrix4 ToMatrix(const Vector3& translation) {
		return {
			1.0f, 0.0f, 0.0f, translation.X(),
			0.0f, 1.0f, 0.0f, translation.Y(),
			0.0f, 0.0f, 1.0f, translation.Z(),
			0.0f, 0.0f, 0.0f, 1.0f,
		};
	}
	static Matrix4 ToMatrix(const Quaternion& rotation) {
		float x2 = rotation.X() * rotation.X();
		float y2 = rotation.Y() * rotation.Y();
		float z2 = rotation.Z() * rotation.Z();
		float xy = rotation.X() * rotation.Y();
		float xz = rotation.X() * rotation.Z();
		float yz = rotation.Y() * rotation.Z();
		float wx = rotation.W() * rotation.X();
		float wy = rotation.W() * rotation.Y();
		float wz = rotation.W() * rotation.Z();
		return {
			1.f-2*(y2+z2), 2*(xy-wz),     2*(xz+wy),     0.0f,
			2*(xy+wz),     1.f-2*(x2+z2), 2*(yz-wx),     0.0f,
			2*(xz-wy),     2*(yz+wx),     1.f-2*(x2+y2), 0.0f,
			0.0f,          0.0f,          0.0f,          1.0f,
		};
	}
	static Matrix4 ToMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale) {
		float x2 = rotation.X() * rotation.X();
		float y2 = rotation.Y() * rotation.Y();
		float z2 = rotation.Z() * rotation.Z();
		float xy = rotation.X() * rotation.Y();
		float xz = rotation.X() * rotation.Z();
		float yz = rotation.Y() * rotation.Z();
		float wx = rotation.W() * rotation.X();
		float wy = rotation.W() * rotation.Y();
		float wz = rotation.W() * rotation.Z();
		return {
			(1.f-2*(y2+z2))*scale.X(), 2*(xy-wz),                 2*(xz+wy),                 translation.X(),
			2*(xy+wz),                 (1.f-2*(x2+z2))*scale.Y(), 2*(yz-wx),                 translation.Y(),
			2*(xz-wy),                 2*(yz+wx),                 (1.f-2*(x2+y2))*scale.Z(), translation.Z(),
			0.0f,                      0.0f,                      0.0f,                      1.0f,
		};
	}

	Transform(): Matrix4{Matrix4::Identity()} {}
	Transform(const Matrix4& m): Matrix4(m) {}
	Transform(const Vector3& translation): Matrix4{ToMatrix(translation)} {}
	Transform(const Quaternion& rotation): Matrix4{ToMatrix(rotation)} {}
	Transform(const Vector3& translation, const Quaternion& rotation, const Vector3& scale = {1.0f}):
		Matrix4{ToMatrix(translation, rotation, scale)} {}

	Point operator()(const Point& p) const {
		return (const Matrix4&)(*this)*(const Vector4&)p;
	}

	// A plane f consists of a normal n and a distance d
	// n is a cross product (i.e. antivector), so nᴮ = nᴬ*Adj(M)
	// the question is what to do with dᴬ to transform it to dᴮ
	// we know that d = -nq, where q is some point on the plane
	// dᴮ = -nᴮqᴮ =
	// = -nᴬAdj(M)(Mqᴬ+t) =
	// = -nᴬDet(M)qᴬ - nᴬAdj(M)t
	// = Det(M)(dᴬ - nᴬM⁻¹t)
	// Except for the extra factor of det(M), this is exactly the value produced
	// by multiplying the plane fᴬ by the fourth column of H⁻¹
	// Using the fact that det(H) = det(M) due to the specific form of H, we come to
	// the conclusion
	// fᴮ = fᴬDet(H)H⁻¹ = fᴬAdj(H)
	Plane operator()(const Plane& f) const {
		return (const Vector4&)f*Adjugate();
	}

	Transform operator*(const Transform& other) const {
		return Transform((const Matrix4&)(*this)*(const Matrix4&)other);
	}
};

inline std::ostream& operator<<(std::ostream& stream, const Transform& t) {
	return stream << "Transform " << (const Matrix4&)t;
}

// see docs/LookAt.ipynb for delails
inline Transform LookAt(const Vector3& eye, const Vector3& center, const Vector3& world_up) {
	Vector3 fwd = (center - eye).Normalize();
	Vector3 right = fwd.Cross(world_up).Normalize();
	Vector3 up = fwd.Cross(right);
	Matrix4 camera_transform {
		right.X(), up.X(), fwd.X(), eye.X(),
		right.Y(), up.Y(), fwd.Y(), eye.Y(),
		right.Z(), up.Z(), fwd.Z(), eye.Z(),
		     0.0f,   0.0f,    0.0f,        1.0f,
	};
	return camera_transform.Inverse();
}

// the projection plane is placed on the distance z=g (focal length)
// it has dimensions x∈[-s, +s] and y∈[-1, +1]
// s is an aspect ration (16/9 for example)
//
// tan(FOVx/2) = s/g
// tan(FOVy/2) = 1/g
//
// FOVy is usually locked to 60deg => g is locked too
//
// the near plane is placed on the distance z=n
// the far plane is placed on the distance z=f
//
// Finding the projection of a point P is determining where
// a line segment connecting the origin to P intersects the projection plane.
//
// Qx = (g / Pz) * Px
// Qy = (g / Pz) * Py
//     ┌         ┐ ┌    ┐   ┌        ┐
//     | g 0 0 0 | | Px | = | g * Px |
// Q = | 0 g 0 0 | | Py | = | g * Py |
//     | 0 0 0 0 | | Pz | = | 0      |
//     | 0 0 1 0 | | 1  | = | Pz     |
//     └         ┘ └    ┘   └        ┘
// The projected y coordinate falls in the range [-1, +1] ,
// but the projected x coordinate falls into the range [-s, +s],
// so we also need to divide the x coordinate by s
// to transform it into the screen space
//     ┌           ┐ ┌    ┐   ┌          ┐
//     | g/s 0 0 0 | | Px | = | g/s * Px |
// Q = |   0 g 0 0 | | Py | = | g   * Py |
//     |   0 0 0 0 | | Pz | = | 0        |
//     |   0 0 1 0 | | 1  | = | Pz       |
//     └           ┘ └    ┘   └          ┘
// Qz must be equal 0 on the near plane where z=n
// and equal 1 on the far plane where z=f
// We can use A and B constants to achieve this
//     ┌           ┐ ┌    ┐   ┌          ┐
//     | g/s 0 0 0 | | Px | = | g/s * Px |
// Q = |   0 g 0 0 | | Py | = | g   * Py |
//     |   0 0 A B | | Pz | = | A*Pz+B   |
//     |   0 0 1 0 | | 1  | = | Pz       |
//     └           ┘ └    ┘   └          ┘
// Which means Qz = A+B/Pz
// To calculate A and B we need to solve the system of equations
// A+B/n=0
// A+B/f=1
// The answer:
// A = f/(f-n)
// B = -nf/(f-n)
// The problem is that the mapping Pz -> Qz is not linear
// When Qz=1/2, Pz=2nf/(f+n)
// Which is close to 2n (and n is expected to be a small number)
// Which means Qz∈[0,1/2) corresponse to a very small distance and Qz∈[1/2, 1) - to very large distance
// But in 32-bit floating-point only one exponent value dedicated for [1/2, 1) range
// The solution is to reverse the mapping range:
// A+B/n=1
// A+B/f=0
// Then
// A = n/(n-f)
// B = -fn/(n-f)
// In this case we need to change depthCompareOp to VK_COMPARE_OP_GREATER
inline Transform Perspective(float FOVy, float s, float n, float f) {
	float g = 1 / std::tan(FOVy/2);
	float A = n/(n-f);
	float B = -f*A;
	return Matrix4 {
		g/s,  0.0f, 0.0f, 0.0f,
		0.0f,    g, 0.0f, 0.0f,
		0.0f, 0.0f,    A,    B,
		0.0f, 0.0f, 1.0f, 0.0f
	};
}

// We need to transform x and y to a range [-1,+1]
// and z to a range [0,1]
inline Transform Ortho(float w, float h, float d) {
	return Matrix4 {
		2/w,  0.0f, 0.0f, 0.0f,
		0.0f,  2/h, 0.0f, 0.0f,
		0.0f, 0.0f,  1/d, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

}

#include "nearly.hpp"

inline bool operator==(const math::Transform& a, const Nearly<math::Transform>& nearly) {
	return ((const math::Matrix4&)a == Nearly((const math::Matrix4&)nearly.t, nearly.epsilon));
}
