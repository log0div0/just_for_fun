
#include "App.hpp"
#include "Utils.hpp"
#include "Stopwatch.hpp"

App::App(glfw::Window& window_): window(window_), render_context(window), camera(window), gui(window)
{
	window.framebufferSizeEvent.subscribe([this](glfw::Window& window, int w, int h) {
		camera.aspect = float(w) / h;
	});
	auto [w, h] = window.getFramebufferSize();
	camera.aspect = float(w) / h;

	camera.pos = {2.0f, 0.0f, 0.0f};

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
					window.setMonitor({}, x, y, this->w, this->h, glfw::dontCare);
				} else {
					std::tie(x,y) = window.getPos();
					std::tie(this->w,this->h) = window.getSize();
					auto monitor = glfw::getPrimaryMonitor();
					const glfw::VideoMode mode = monitor.getVideoMode();
					window.setMonitor(monitor, 0, 0, mode.width, mode.height, mode.refreshRate);
				}
				break;
		};
	});
}

App::~App()
{
	render_context.WaitIdle();
}

void App::Run()
{
	float delta_time = 0.0f;
	while (!window.shouldClose())
	{
		glfw::pollEvents();

		Stopwatch timer;

		Update(delta_time);
		Render();

		delta_time = timer.GetFrameTime(gui.limit_framerate);
	}
}

void App::Update(float delta_time)
{
	camera.Update(delta_time);
	box.Update(delta_time);
	light.Update(delta_time);
	gui.Update(delta_time, light);
}

void App::Render()
{
	render_context.Clear();

	light.Render(camera);
	box.Render(camera, light);
	gui.Render();

	render_context.Present();
}
