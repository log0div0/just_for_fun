
#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef _GAMING_XBOX
#include <winapi/ComPtr.hpp>
#include <GameInput.h>
#else
#include <glfwpp/glfwpp.h>
#endif

#include <math/Vector.hpp>

#include <functional>

struct Window {
	Window(const std::string& name);
	~Window();

	Window(const Window& other) = delete;
	Window& operator=(const Window& other) = delete;

	Window(Window&& other) = delete;
	Window& operator=(Window&& other) = delete;

	float CameraForwardSpeed();
	float CameraRightSpeed();
	float CameraUpSpeed();
	std::pair<float, float> CameraRotationSpeed();

	void Update(float delta_time);
	bool ShouldClose();

	void OnWindowResize(std::function<void(int, int)> cb);
	std::tuple<int, int> GetWindowSize();

#ifdef _GAMING_XBOX
	winapi::ComPtr<IGameInput> game_input;
	GameInputGamepadState gamepad_state;
	bool should_close = false;
#else
	glfw::Window& GetGLFW() {
		return window;
	}

private:
	int x, y, w, h;
	using CursorPos = math::Vector<double, 2>;
	CursorPos prev_cursor_pos;
	CursorPos current_cursor_pos;
	glfw::Window window;
#endif
};
