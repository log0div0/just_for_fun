
#pragma once

#ifndef _GAMING_XBOX
#include <Windows.h>
#include <glfwpp/glfwpp.h>
#endif

#include <math/Vector.hpp>

struct Window {
	Window(const std::string& name);

	Window(const Window& other) = delete;
	Window& operator=(const Window& other) = delete;

	Window(Window&& other) = delete;
	Window& operator=(Window&& other) = delete;

	bool IsCameraMovingForward();
	bool IsCameraMovingBack();
	bool IsCameraMovingRight();
	bool IsCameraMovingLeft();
	bool IsCameraMovingUp();
	bool IsCameraMovingDown();
	void OnCameraRotate(std::function<void(float, float)> cb);
	void OnCameraAccelerate(std::function<void(float)> cb);

	void Update(float delta_time);
	bool ShouldClose();

	void OnWindowResize(std::function<void(int, int)> cb);
	std::tuple<int, int> GetWindowSize();

#ifndef _GAMING_XBOX
	glfw::Window& GetGLFW() {
		return window;
	}

private:
	int x, y, w, h;
	using CursorPos = math::Vector<double, 2>;
	CursorPos last_cursor_pos;
	glfw::Window window;
#endif
};
