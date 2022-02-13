
#pragma once

#include <array>
#include <ostream>
#include <numbers>

namespace math {

constexpr float RadiansToDegrees(float radians) {
	return radians / std::numbers::pi_v<float> * 180.0f;
}

constexpr float DegreesToRadians(float degrees) {
	return degrees / 180.0f * std::numbers::pi_v<float>;
}

namespace literals {

constexpr float operator"" _deg (long double deg) {
	return math::DegreesToRadians(deg);
}

constexpr float operator"" _deg (unsigned long long int deg) {
	return math::DegreesToRadians(deg);
}

}

using namespace literals;

template <typename T, size_t N>
struct Vector {
	Vector(): data{} {}

	Vector(T t) {
		data.fill(t);
	}

	template <typename... Args>
	Vector(Args&&... args) requires (sizeof...(Args) == N): data{std::forward<Args>(args)...} {}

	T& operator[](size_t i) {
		return data[i];
	}

	const T& operator[](size_t i) const {
		return data[i];
	}

	float Magnitude() const {
		const Vector& v = *this;
		return std::sqrt(v.Dot(v));
	}

	Vector Normalize() const {
		const Vector& v = *this;
		return v / Magnitude();
	}

	//
	// a•b = |a||b|cosθ
	// => dot product equals 0 if vectors are perpendicular
	//
	T Dot(const Vector& b) const {
		const Vector& a = *this;
		T sum = 0;
		for (size_t i = 0; i < N; ++i) {
			sum += a[i] * b[i];
		}
		return sum;
	}

	bool operator==(const Vector& b) const {
		return data == b.data;
	}

	float Cos(const Vector& b) const {
		const Vector& a = *this;
		return a.Dot(b) / a.Magnitude() / b.Magnitude();
	}

	//
	// proj(a,b) = norm(b) * (|a| * cosθ) =
	// = (b/|b|) * (|a| * a•b/|a|/|b|) =
	// = b*a•b / |b|² =
	// = b*(a•b / b•b)
	//
	Vector ProjectOnto(const Vector& b) const {
		const Vector& a = *this;
		return b.Normalize() * (a.Magnitude() * a.Cos(b));
	}

	Vector RejectFrom(const Vector& b) const {
		const Vector& a = *this;
		return a - a.ProjectOnto(b);
	}

	Vector Rotate(float angle, Vector axis) const requires (N == 3) {
		const Vector& v = *this;
		return
			v.ProjectOnto(axis) +						// projection onto the axis remains unmodified
			v.RejectFrom(axis) * std::cos(angle) +		// rejection remains unmodified if angle==0 and disappears completly if angle==PI/2
			axis.Cross(v) * std::sin(angle);			// cross product is perpendicular to both projection and rejection
														// the order of vectors in cross product means that rotation is counterclockwise
	}

	const T& X() const requires (N > 0) {
		return data[0];
	}

	const T& Y() const requires (N > 1) {
		return data[1];
	}

	const T& Z() const requires (N > 2) {
		return data[2];
	}

	Vector operator+(const Vector& b) const {
		const Vector& a = *this;
		Vector c;
		for (size_t i = 0; i < N; ++i) {
			c[i] = a[i] + b[i];
		}
		return c;
	}

	Vector operator-(const Vector& b) const {
		const Vector& a = *this;
		Vector c;
		for (size_t i = 0; i < N; ++i) {
			c[i] = a[i] - b[i];
		}
		return c;
	}

	Vector operator-() const {
		Vector c;
		for (size_t i = 0; i < N; ++i) {
			c[i] = -data[i];
		}
		return c;
	}

	Vector operator*(T b) const {
		const Vector& a = *this;
		Vector c;
		for (size_t i = 0; i < N; ++i) {
			c[i] = a[i] * b;
		}
		return c;
	}

	friend Vector operator*(T t, const Vector& v) {
		return v * t;
	}

	Vector operator/(T b) const {
		const Vector& a = *this;
		Vector c;
		for (size_t i = 0; i < N; ++i) {
			c[i] = a[i] / b;
		}
		return c;
	}

	friend Vector operator/(T t, const Vector& v) {
		return v / t;
	}

	//
	// a cross product is a special case of the wedge product (or exterior product):
	// more precisely cross product is a complement of a wedge product
	//
	//	a^b=(AxE1+AyE2+AzE3)^(BxE1+ByE2+BzE3)=
	//
	// = AxBx(E1^E1) + AxBy(E1^E2) + AxBz(E1^E3)
	// + AyBx(E2^E1) + AyBy(E2^E2) + AyBz(E2^E3)
	// + AzBx(E3^E3) + AzBy(E3^E2) + AzBz(E3^E3)

	// =      0      + AxBy(E1^E2) - AxBz(E3^E1)
	// - AyBx(E1^E2) +      0      + AyBz(E2^E3)
	// + AzBx(E3^E1) - AzBy(E2^E3) +      0
	//
	// = (AyBz-AzBy)(E2^E3)
	// + (AzBx-AxBz)(E3^E1)
	// + (AxBy-AyBx)(E1^E2)
	//
	// ___               _____                _____                _____
	// a^b = (AyBz-AzBy)(E2^E3) + (AzBx-AxBz)(E3^E1) + (AxBy-AyBx)(E1^E2) =
	//     = (AyBz-AzBy)E1      + (AzBx-AxBz)E2      + (AxBy-AyBx)E3      =
	// = a⨯b
	//
	// Wedge prouct was introduced in Grassmann Algebra and then generalized in Clifford Algebra along with quaternions and etc
	//
	// |a⨯b| = |a||b|sinθ
	// => a magnitude of a cross product equals 0 if vectors are parallel
	//
	// In a right-handed coordinate system, the cross product follows the right-hand
	// rule. When the fingers of the right hand curl in the direction of the smallest angle from the
	// vector a to the vector b, the right thumb points in the direction of a⨯b.
	//
	Vector Cross(const Vector& b) const requires (N == 3) {
		const Vector& a = *this;
		return {
			a.Y()*b.Z() - a.Z()*b.Y(),
			a.Z()*b.X() - a.X()*b.Z(),
			a.X()*b.Y() - a.Y()*b.X(),
		};
	}

	std::array<T, N> data = {};
};

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& stream, const Vector<T, N>& v) {
	stream << "(";
	for (size_t i = 0; i < N; ++i) {
		if (i) {
			stream << ", ";
		}
		stream << v[i];
	}
	return stream << ")";
}


using Vector2 = Vector<float, 2>;
using Vector3 = Vector<float, 3>;
using Vector4 = Vector<float, 4>;

}

#include "nearly.hpp"

template <typename T, size_t N>
bool operator==(const math::Vector<T, N>& a, const Nearly<math::Vector<T, N>>& nearly) {
	for (size_t i = 0; i < a.data.size(); ++i) {
		if (!(a.data[i] == Nearly(nearly.t.data[i], nearly.epsilon))) {
			return false;
		}
	}
	return true;
}