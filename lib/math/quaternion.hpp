
#pragma once

#include "vector.hpp"

namespace math {

// i² = j² = k² = ijk = -1
// ij = -ji = k
// jk = -kj = i
// ki = -ik = j

// q = xi + yj + zk + w
struct Quaternion {
	Vector3 v;
	float s = 0.0f;

	Quaternion() = default;
	Quaternion(const Vector3& v_, float s_): v(v_), s(s_) {}
	Quaternion(float x, float y, float z, float w): v(x, y, z), s(w) {}

	// magnitude property:
	// |q1*q2| = |q1|*|q2|
	Quaternion operator*(const Quaternion& b) const {
		const Quaternion& a = *this;

		Quaternion result;
		result.v = a.v.Cross(b.v) +
			a.v*b.s +
			b.v*a.s;

		result.s = a.s*b.s - a.v.Dot(b.v);

		return result;
	}

	Quaternion operator/(float f) const {
		return {v/f, s/f};
	}

	// Conjugate is used to zero a vector part:
	// q*Conj(q) = v•v + s*s
	Quaternion Conjugate() const {
		return {-v, s};
	}

	float Magnitude() const {
		const Quaternion& q = *this;
		return std::sqrt((q * q.Conjugate()).s);
	}

	// A quatemion q having a magnitude of one is called a unit quaternion
	// for a unit quaternion Inv(q) == Conj(q)
	Quaternion Normalize() const {
		const Quaternion& q = *this;
		return q / Magnitude();
	}

	// q*Conj(q)/q*Conj(q) = 1
	Quaternion Inverse() const {
		const Quaternion& q = *this;
		return q.Conjugate() / (q * q.Conjugate()).s;
	}

	// When v is a pure quaternion, the sandwich product
	// qvq⁻¹ always yields another pure quatemion
	Vector3 Rotate(const Vector3& v) const {
		const Quaternion& q = *this;
		return (q * Quaternion(v, 0) * q.Inverse()).v;
	}

	static Quaternion MakeRotation(float angle, const Vector3& axis) {
		return {axis.Normalize() * std::sin(angle / 2), std::cos(angle / 2)};
	}

	// advantages of quaternions:
	// 1) the two successive rotations q1 and q2 are equivalent to a single rotation using the quaternion q2*q1
	//    which has a significantly lower cost than a matrix multiplication
	// 2) it has much lower storage requirements
	// 3) we can use interpolation on quaterions

	// disadvantages of quaternions:
	// 1) useful only for rotations
	// 2) rotation of vectors is more expensive that multiplying a vector by a matrix
};

inline std::ostream& operator<<(std::ostream& stream, const Quaternion& q) {
	return stream << "Quaternion [" << q.v << " | " << q.s << "]";
}

}

#include "nearly.hpp"

inline bool operator==(const math::Quaternion& a, const Nearly<math::Quaternion>& nearly) {
	return (a.v == Nearly(nearly.t.v, nearly.epsilon)) && (a.s == Nearly(nearly.t.s, nearly.epsilon));
}
