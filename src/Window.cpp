#include "Window.hpp"

#ifndef _GAMING_XBOX

Window::Window(const std::string& name): x(0), y(0), w(1200), h(900)
{
	window = glfw::Window {w, h, name.c_str()};
	window.keyEvent.subscribe([this](glfw::Window& window, glfw::KeyCode key, int scancode, glfw::KeyState action, glfw::ModifierKeyBit mods)
	{
		if (action != glfw::KeyState::Press) {
			return;
		}
		switch (key) {
			case glfw::KeyCode::Escape:
				window.setShouldClose(true);
				break;
			case glfw::KeyCode::F11:
				if ((GLFWmonitor*)window.getMonitor()) {
					window.setMonitor({}, x, y, w, h, glfw::dontCare);
				} else {
					std::tie(x,y) = window.getPos();
					std::tie(w,h) = window.getSize();
					auto monitor = glfw::getPrimaryMonitor();
					const glfw::VideoMode mode = monitor.getVideoMode();
					window.setMonitor(monitor, 0, 0, mode.width, mode.height, mode.refreshRate);
				}
				break;
		};
	});
	last_cursor_pos = std::make_from_tuple<CursorPos>(window.getCursorPos());
}

bool Window::IsCameraMovingForward() {
	return window.getKey(glfw::KeyCode::W);
}

bool Window::IsCameraMovingBack() {
	return window.getKey(glfw::KeyCode::S);
}

bool Window::IsCameraMovingRight() {
	return window.getKey(glfw::KeyCode::D);
}

bool Window::IsCameraMovingLeft() {
	return window.getKey(glfw::KeyCode::A);
}

bool Window::IsCameraMovingUp() {
	return window.getKey(glfw::KeyCode::E);
}

bool Window::IsCameraMovingDown() {
	return window.getKey(glfw::KeyCode::Q);
}

void Window::OnCameraRotate(std::function<void(float, float)> cb) {
	window.cursorPosEvent.subscribe([this, cb](glfw::Window& window, double x, double y) {
		CursorPos pos{x, y};
		if (window.getMouseButton(glfw::MouseButton::Right)) {
			CursorPos diff = last_cursor_pos - pos;
			cb(diff.X(), diff.Y());
		}
		last_cursor_pos = pos;
	});
}

void Window::OnCameraAccelerate(std::function<void(float)> cb) {
	window.scrollEvent.subscribe([this, cb](glfw::Window& window, double x, double y) {
		cb(y);
	});
}

void Window::Update(float delta_time) {
	glfw::pollEvents();
}

bool Window::ShouldClose() {
	return window.shouldClose();
}

void Window::OnWindowResize(std::function<void(int, int)> cb) {
	window.framebufferSizeEvent.subscribe([this, cb](glfw::Window& window, int w, int h) {
		cb(w, h);
	});
}

std::tuple<int, int> Window::GetWindowSize() {
	return window.getFramebufferSize();
}

#endif
