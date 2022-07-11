#include "Window.hpp"

#ifdef _GAMING_XBOX

#include <XGameRuntime.h>
#include <winapi/Functions.hpp>

Window::Window(const std::string& name)
{
	THROW_IF_FAILED(XGameRuntimeInitialize());
	THROW_IF_FAILED(GameInputCreate(&game_input));
}

Window::~Window() {
	XGameRuntimeUninitialize();
}

float Window::CameraForwardSpeed() {
	if ((gamepad_state.leftThumbstickY < -0.1f) || (gamepad_state.leftThumbstickY > 0.1)) {
		return gamepad_state.leftThumbstickY;
	}
	return 0;
}

float Window::CameraRightSpeed() {
	if ((gamepad_state.leftThumbstickX < -0.1f) || (gamepad_state.leftThumbstickX > 0.1)) {
		return gamepad_state.leftThumbstickX;
	}
	return 0;
}

float Window::CameraUpSpeed() {
	if (gamepad_state.buttons & GameInputGamepadRightShoulder) {
		return 1.0f;
	} else if (gamepad_state.buttons & GameInputGamepadLeftShoulder) {
		return -1.0f;
	}
	return 0.0f;
}

std::pair<float, float> Window::CameraRotationSpeed() {
	float dx = 0;
	float dy = 0;
	if ((gamepad_state.rightThumbstickX < -0.1f) || (gamepad_state.rightThumbstickX > 0.1)) {
		dx = -gamepad_state.rightThumbstickX;
	}
	if ((gamepad_state.rightThumbstickY < -0.1f) || (gamepad_state.rightThumbstickY > 0.1)) {
		dy = gamepad_state.rightThumbstickY;
	}
	return {dx, dy};
}

void Window::Update(float delta_time) {
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (WM_QUIT == msg.message) {
			should_close = true;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	gamepad_state = {};

	winapi::ComPtr<IGameInputReading> game_input_reading;
	if (FAILED(game_input->GetCurrentReading(GameInputKindGamepad, nullptr, &game_input_reading)))
	{
		// Failure indicates no gamepad is connected
	}
	else
	{
		game_input_reading->GetGamepadState(&gamepad_state);
	}
}

bool Window::ShouldClose() {
	return should_close;
}

void Window::OnWindowResize(std::function<void(int, int)> cb) {
}

std::tuple<int, int> Window::GetWindowSize() {
	switch (XSystemGetDeviceType())
	{
		case XSystemDeviceType::XboxOne:
		case XSystemDeviceType::XboxOneS:
		case XSystemDeviceType::XboxScarlettLockhart /* Xbox Series S */:
			return { 1920, 1080 };
		default:
			return { 3840, 2160 };
	}
}

#else

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
	prev_cursor_pos = current_cursor_pos = std::make_from_tuple<CursorPos>(window.getCursorPos());
}

Window::~Window() {

}

float Window::CameraForwardSpeed() {
	if (window.getKey(glfw::KeyCode::W)) {
		return 1.0f;
	} else if (window.getKey(glfw::KeyCode::S)) {
		return -1.0f;
	}
	return 0.0f;
}

float Window::CameraRightSpeed() {
	if (window.getKey(glfw::KeyCode::D)) {
		return 1.0f;
	} else if (window.getKey(glfw::KeyCode::A)) {
		return -1.0f;
	}
	return 0.0f;
}

float Window::CameraUpSpeed() {
	if (window.getKey(glfw::KeyCode::E)) {
		return 1.0f;
	} else if (window.getKey(glfw::KeyCode::Q)) {
		return -1.0f;
	}
	return 0.0f;
}

std::pair<float, float> Window::CameraRotationSpeed() {
	if (window.getMouseButton(glfw::MouseButton::Right)) {
		CursorPos diff = (prev_cursor_pos - current_cursor_pos) / 3;
		return {(float)diff.X(), (float)diff.Y()};
	}
	return {};
}

void Window::Update(float delta_time) {
	glfw::pollEvents();
	prev_cursor_pos = current_cursor_pos;
	current_cursor_pos = std::make_from_tuple<CursorPos>(window.getCursorPos());
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
