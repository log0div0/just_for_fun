
#pragma once

#include <Windows.h>
#include <stdexcept>

namespace winapi {

template <typename T>
struct ComPtr {
	ComPtr() = default;
	~ComPtr() {
		if (t) {
			t->Release();
		}
	}

	ComPtr(const ComPtr& other);
	ComPtr& operator=(const ComPtr& other);

	ComPtr(ComPtr&& other): t(other.t) {
		other.t = nullptr;
	}

	ComPtr& operator=(ComPtr&& other) {
		std::swap(t, other.t);
		return *this;
	}

	template <typename U>
	ComPtr<U> QueryInterface() const {
		ComPtr<U> u;
		if (t->QueryInterface(&u) != S_OK) {
			throw std::runtime_error("QueryInterface failed");
		}
		return u;
	}

	T* Get() {
		return t;
	}
	const T* Get() const {
		return t;
	}

	T* operator->() {
		return t;
	}
	const T* operator->() const {
		return t;
	}

	T** operator&() {
		return &t;
	}
	const T* const * operator&() const {
		return &t;
	}

	operator T*() {
		return t;
	}

	operator const T*() {
		return t;
	}

	T* t = nullptr;
};

}
