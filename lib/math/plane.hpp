
#pragma once

#include "line.hpp"

namespace math {

struct Point: Vector3 {
	using Vector3::Vector3;

	Point(const Vector3& v): Vector3(v), w(1.0f) {}
	Point(const Vector4& v): Vector3{v[0], v[1], v[2]}, w(v[3]) {}

	operator const Vector4&() const {
		static_assert(sizeof(*this) == sizeof(Vector4));
		return *(Vector4*)this;
	}

	float w = 1.0f;
};

struct Direction: Vector3 {
	using Vector3::Vector3;

	Direction(const Vector3& v): Vector3(v), w(0.0f) {}
	Direction(const Vector4& v): Vector3{v[0], v[1], v[2]}, w(v[3]) {}

	operator const Vector4&() const {
		static_assert(sizeof(*this) == sizeof(Vector4));
		return *(Vector4*)this;
	}

	float w = 0.0f;
};

struct Plane {
	Plane(const Point& p1, const Point& p2, const Point& p3): Plane(p1, Direction(p2-p1), Direction(p3-p1)) {
	}

	// Plane can be described as
	// Q(s, t) = q + su + tv
	// where u and v - vectors that both lie in the plane
	Plane(const Point& q, const Direction& u, const Direction& v): Plane(q, u.Cross(v)) {
	}

	// We can calculate n = u⨯v
	// This normal vector is perpendicular to the difference between any two
	// points in the plane, so the plane can be described as the equation
	// n•(p-q)=0
	// where p is an arbitrary point lying in the plane
	Plane(const Point& q, const Vector3& n): Plane(n, -n.Dot(q)) {}

	// Let's expand brackets
	// n•p-n•q = 0
	// There -n•q is a constant value that we call d:
	// n•p+d=0
	// If we multiply both sides of the equation by a scalar s
	// we get the same plane. Let's say s=1/|n|
	// Then n is a normalized vector and d is a distance to the origin
	Plane(const Vector3& n_, float d_): n(n_ / n_.Magnitude()), d(d_ / n_.Magnitude()) {}

	Plane(const Vector4& v): Plane({v[0], v[1], v[2]}, v[3]) {}

	//
	// dist([n|d],p) = |proj(p,n)|+d =
	// = |n*p•n/n•n|+d =
	// as n is a normalized vector
	// = p•n+d
	//
	float GetDistanceTo(const Vector3& p) const {
		return n.Dot(p) + d;
	}

	bool IsOnFrontSide(const Vector3& p) const {
		return GetDistanceTo(p) > 0.0f;
	}

	bool IsParallelTo(const Line& line) const {
		return fabs(n.Dot(line.v)) < std::numeric_limits<float>::epsilon();
	}

	// We neet to find such a point q at the line L(t)=p+tv that
	// dist([n|d], q) = 0
	// n•q + d = 0
	// n•(p+tv) + d = 0
	// n•p+tn•v + d = 0
	// t = -(n•p+d)/n•v
	// Plugging this value of t back into L(t) gives us
	// q = p - ((n•p+d)/n•v)v
	std::optional<Vector3> GetIntersectionWith(const Line& line) {
		if (IsParallelTo(line)) {
			return std::nullopt;
		}

		const Vector3& p = line.p;
		const Vector3& v = line.v;

		float t = -GetDistanceTo(p)/n.Dot(v);

		return line.GetPoint(t);
	}

	operator const Vector4&() const {
		static_assert(sizeof(*this) == sizeof(Vector4));
		return *(Vector4*)this;
	}

	Vector3 n;
	float d;
};

inline std::ostream& operator<<(std::ostream& stream, const Plane& plane) {
	return stream << "Plane [" << plane.n << " | " << plane.d << "]";
}

}

#include "nearly.hpp"

inline bool operator==(const math::Point& a, const Nearly<math::Point>& nearly) {
	return (const math::Vector4&)a == Nearly((const math::Vector4&)nearly.t, nearly.epsilon);
}

inline bool operator==(const math::Direction& a, const Nearly<math::Direction>& nearly) {
	return (const math::Vector4&)a == Nearly((const math::Vector4&)nearly.t, nearly.epsilon);
}

inline bool operator==(const math::Plane& a, const Nearly<math::Plane>& nearly) {
	return (a.n == Nearly(nearly.t.n, nearly.epsilon)) && (a.d == Nearly(nearly.t.d, nearly.epsilon));
}