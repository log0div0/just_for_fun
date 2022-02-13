
#pragma once

#include <iostream>
#include <type_traits>

template <typename T>
struct Nearly {
	Nearly(const T& t_, double epsilon_ = 0.0001): t(t_), epsilon(epsilon_) {}

	T t;
	double epsilon;
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, const Nearly<T>& nearly) {
	return stream << nearly.t;
}

inline bool operator==(float f, const Nearly<float>& nearly) {
	double diff = std::abs(f - nearly.t);
	return diff < nearly.epsilon;
}
