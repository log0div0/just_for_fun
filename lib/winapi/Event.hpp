
#pragma once

#include "Object.hpp"

namespace winapi {

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
