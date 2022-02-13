
#pragma once

#include "matrix.hpp"

namespace math {

struct Line {
	// Line can be described as
	// L(t) = p + tv
	Line(const Vector3& point, const Vector3& dir): p(point), v(dir) {}

	Vector3 GetPoint(float t) const {
		return p + t*v;
	}

	float GetDistanceTo(const Vector3& q) const {
		Vector3 u = q - p;
		return u.RejectFrom(v).Magnitude();
	}

	bool IsParallelTo(const Line& l2) const {
		return fabs(v.Cos(l2.v) - 1.0f) < std::numeric_limits<float>::epsilon();
	}

	// the shortest distance d between two lines is the length of the segment L2(t2) - L1(t1) that
	// is simultaneously perpendicular to both lines:
	//
	// (L2(t2) - L1(t1))•v1=0
	// (L2(t2) - L1(t1))•v2=0
	//
	// (p2+t2v2-p1-t1v1)•v1=0
	// (p2+t2v2-p1-t1v1)•v2=0
	//
	// move everything without t1/t2 to the right side
	//
	// t2v1•v2-t1v1•v1=(p1-p2)•v1
	// t2v2•v2-t1v1•v2=(p1-p2)•v2
	//
	// express left side via matrix multiplication
	// ┌              ┐ ┌    ┐   ┌            ┐
	// | v1•v2 -v1•v1 |*| t2 | = | (p1-p2)•v1 |
	// | v2•v2 -v1•v2 | | t1 |   | (p1-p2)•v2 |
	// └              ┘ └    ┘   └            ┘
	// ┌    ┐   ┌              ┐-1 ┌            ┐
	// | t2 | = | v1•v2 -v1•v1 |  *| (p1-p2)•v1 |
	// | t1 |   | v2•v2 -v1•v2 |   | (p1-p2)•v2 |
	// └    ┘   └              ┘   └            ┘
	float GetDistanceTo(const Line& l2) const {
		if (IsParallelTo(l2)) {
			// if lines are parallel a determinant of the matrix below
			// is zero, so we can't calculate an inverse matrix of it
			return GetDistanceTo(l2.p);
		}

		const Vector3& p1 = p;
		const Vector3& v1 = v;
		const Vector3& p2 = l2.p;
		const Vector3& v2 = l2.v;

		Matrix2 a = {
			v1.Dot(v2), -v1.Dot(v1),
			v2.Dot(v2), -v1.Dot(v2)
		};
		Vector2 b = {
			(p1-p2).Dot(v1),
			(p1-p2).Dot(v2)
		};
		Vector2 t = a.Inverse() * b;
		return (l2.GetPoint(t[0]) - GetPoint(t[1])).Magnitude();
	}

private:
	friend struct Plane;

	Vector3 p, v;
};

}
