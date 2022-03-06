
#pragma once

#include <Windows.h>
#include <stdexcept>

namespace winapi {

struct Object {
	Object() = default;

	explicit Object(HANDLE handle_): handle(handle_) {
		if (!handle) {
			throw std::runtime_error("Construct a winapi object with a null handle");
		}
	}

	~Object() {
		if (handle != INVALID_HANDLE_VALUE) {
			CloseHandle(handle);
		}
	}

	Object(const Object& other) = delete;
	Object& operator=(const Object& other) = delete;

	Object(Object&& other): handle(other.handle) {
		other.handle = nullptr;
	}

	Object& operator=(Object&& other) {
		std::swap(handle, other.handle);
		return *this;
	}

	DWORD Wait(DWORD ms) {
		return WaitForSingleObject(handle, ms);
	}

	HANDLE handle = INVALID_HANDLE_VALUE;
};

struct Event: Object {
	using Object::Object;

	Event(BOOL manual_reset = FALSE, BOOL is_signaled = FALSE) {
		handle = CreateEvent(NULL, manual_reset, is_signaled, NULL);
		if (!handle) {
			throw std::runtime_error("CreateEvent failed");
		}
	}

	void Set() {
		SetEvent(handle);
	}
};

}
