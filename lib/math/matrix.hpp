
#pragma once

#include <array>
#include <ostream>
#include "vector.hpp"

namespace math {

template <typename T, size_t N, size_t M>
struct Matrix {
	static Matrix Identity() requires (N == M) {
		Matrix m;
		for (size_t i = 0; i < N; ++i) {
			m.At(i, i) = 1;
		}
		return m;
	}

	Matrix(): data{} {}

	Matrix(const Vector<T, N*M>& v): Matrix(v.data) {}

	template <typename... Args>
	Matrix(Args&&... args) requires (sizeof...(Args) == N*M): Matrix(std::array<T, N*M>{std::forward<Args>(args)...}) {}

	Matrix(const std::array<T, N*M>& tmp) {
		for (size_t row = 0; row < N; ++row) {
			for (size_t col = 0; col < M; ++col) {
				size_t row_major = row*M+col;
				size_t col_major = col*N+row;
				data[col_major] = tmp[row_major];
			}
		}
	}

	T& At(size_t row, size_t col) {
		return data[col*N + row];
	}
	const T& At(size_t row, size_t col) const {
		return data[col*N + row];
	}

	Vector<T, M> Row(size_t i) const {
		Vector<T, M> v;
		for (size_t j = 0; j < M; ++j) {
			v[j] = At(i, j);
		}
		return v;
	}
	Vector<T, N> Column(size_t j) const {
		Vector<T, N> v;
		for (size_t i = 0; i < N; ++i) {
			v[i] = At(i, j);
		}
		return v;
	}
	Matrix<T, M, N> Transpose() const {
		Matrix<T, M, N> result;
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < M; ++j) {
				result.At(j, i) = At(i, j);
			}
		}
		return result;
	}

	Matrix<T, N-1, M-1> Remove(size_t remove_row, size_t remove_col) const {
		Matrix<T, N-1, M-1> result;
		size_t sub_row = 0;
		for (size_t i = 0; i < N; ++i) {
			if (i == remove_row) {
				continue;
			}
			size_t sub_col = 0;
			for (size_t j = 0; j < M; ++j) {
				if (j == remove_col) {
					continue;
				}
				result.At(sub_row, sub_col) = At(i, j);
				++sub_col;
			}
			++sub_row;
		}
		return result;
	}

	T Determinant() const requires (N == M && N == 2) {
		return At(0,0)*At(1,1) - At(0,1)*At(1,0);
	}

	T Determinant() const requires (N == M && N == 3) {
		Vector<T, 3> a = Column(0);
		Vector<T, 3> b = Column(1);
		Vector<T, 3> c = Column(2);

		return a.Cross(b).Dot(c);
	}

	T Determinant() const requires (N == M && N >= 4) {
		if constexpr (N == 1) {
			return data[0];
		} else {
			T det = 0;
			for (size_t col = 0; col < M; ++col) {
				T x = At(0, col) * Remove(0, col).Determinant();
				if (col % 2) {
					x *= -1;
				}
				det += x;
			}
			return det;
		}
	}

	Matrix Cofactor() const requires (N == M && N == 2) {
		return {
			 At(1,1), -At(1,0),
			-At(0,1),  At(0,0)
		};
	}

	Matrix Cofactor() const requires (N == M && N == 3) {
		Vector<T, 3> a = Column(0);
		Vector<T, 3> b = Column(1);
		Vector<T, 3> c = Column(2);

		Vector<T, 3> c0 = b.Cross(c);
		Vector<T, 3> c1 = c.Cross(a);
		Vector<T, 3> c2 = a.Cross(b);

		return {
			c0[0], c1[0], c2[0],
			c0[1], c1[1], c2[1],
			c0[2], c1[2], c2[2],
		};
	}

	Matrix Cofactor() const requires (N == M && N >= 4) {
		Matrix<T, N, M> result;
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < M; ++j) {
				result.At(i, j) = Remove(i, j).Determinant();
				if ((i+j) % 2) {
					result.At(i, j) *= -1;
				}
			}
		}
		return result;
	}

	Matrix Adjugate() const requires (N == M) {
		return Cofactor().Transpose();
	}

	// for 3x3 matrix
	//         ┌     ┐
	//         | b⨯c |
	// Inv(M) =| c⨯a | / [a,b,c]
	//         | a⨯b |
	//         └     ┘
	// where a,b,c - columns of M, and [a,b,c] - the scalar triple product of columns
	Matrix Inverse() const requires (N == M) {
		return Adjugate() / Determinant();
	}

	Matrix operator*(T value) const {
		Matrix m;
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < M; ++j) {
				m.At(i, j) = At(i, j) * value;
			}
		}
		return m;
	}

	Matrix operator/(T value) const {
		Matrix m;
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < M; ++j) {
				m.At(i, j) = At(i, j) / value;
			}
		}
		return m;
	}

	Matrix operator+(Matrix b) const {
		Matrix m;
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < M; ++j) {
				m.At(i, j) = At(i, j) + b.At(i, j);
			}
		}
		return m;
	}

	Matrix operator-(Matrix b) const {
		Matrix m;
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < M; ++j) {
				m.At(i, j) = At(i, j) - b.At(i, j);
			}
		}
		return m;
	}

	std::array<T, N*M> data;
};

template <typename T, size_t N, size_t P, size_t M>
Matrix<T, N, M> operator*(const Matrix<T, N, P>& a, const Matrix<T, P, M>& b) {
	Matrix<T, N, M> c;
	for (size_t i = 0; i < N; ++i) {
		for (size_t j = 0; j < M; ++j) {
			c.At(i, j) = a.Row(i).Dot(b.Column(j));
		}
	}
	return c;
}

template <typename T, size_t N, size_t M>
Vector<T, N> operator*(const Matrix<T, N, M>& a, const Vector<T, M>& b) {
	Vector<T, N> c;
	for (size_t i = 0; i < N; ++i) {
		c[i] = a.Row(i).Dot(b);
	}
	return c;
}

template <typename T, size_t N, size_t M>
Vector<T, N> operator*(const Vector<T, M>& a, const Matrix<T, N, M>& b) {
	Vector<T, N> c;
	for (size_t i = 0; i < N; ++i) {
		c[i] = a.Dot(b.Column(i));
	}
	return c;
}

template <typename T, size_t N, size_t M>
Matrix<T, N, M> OuterProduct(const Vector<T, N>& a, const Vector<T, M>& b) {
	return Matrix<T, N, 1>(a) * Matrix<T, 1, M>(b);
}

template <typename T, size_t N, size_t M>
Matrix<T, N, M> operator*(T val, const Matrix<T, N, M>& m) {
	Matrix<T, N, M> c;
	for (size_t i = 0; i < N; ++i) {
		for (size_t j = 0; j < M; ++j) {
			c.At(i, j) = val * m.At(i, j);
		}
	}
	return c;
}

template <typename T, size_t N, size_t M>
Matrix<T, N, M> operator/(T val, const Matrix<T, N, M>& m) {
	Matrix<T, N, M> c;
	for (size_t i = 0; i < N; ++i) {
		for (size_t j = 0; j < M; ++j) {
			c.At(i, j) = val / m.At(i, j);
		}
	}
	return c;
}

template <typename T, size_t N, size_t M>
bool operator==(const Matrix<T, N, M>& a, const Matrix<T, N, M>& b) {
	return a.data == b.data;
}

template <typename T, size_t N, size_t M>
std::ostream& operator<<(std::ostream& stream, const Matrix<T, N, M>& m) {
	stream << "(";
	for (size_t i = 0; i < N; ++i) {
		if (i) {
			stream << std::endl;
		}
		for (size_t j = 0; j < M; ++j) {
			if (j) {
				stream << ", ";
			}
			stream << m.At(i, j);
		}
	}
	return stream << ")";
}

using Matrix2 = Matrix<float, 2, 2>;
using Matrix3 = Matrix<float, 3, 3>;
using Matrix4 = Matrix<float, 4, 4>;

}

#include "nearly.hpp"

template <typename T, size_t N, size_t M>
bool operator==(const math::Matrix<T, N, M>& a, const Nearly<math::Matrix<T, N, M>>& nearly) {
	for (size_t i = 0; i < a.data.size(); ++i) {
		if (!(a.data[i] == Nearly(nearly.t.data[i], nearly.epsilon))) {
			return false;
		}
	}
	return true;
}
