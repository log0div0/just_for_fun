
#include "App.hpp"
#include "Utils.hpp"
#include "Stopwatch.hpp"

App::App(glfw::Window& window_): window(window_), render_context(window), camera(window), gui(window)
{
	window.framebufferSizeEvent.subscribe([&](glfw::Window& window, int w, int h) {
		camera.aspect = float(w) / h;
	});
	auto [w, h] = window.getFramebufferSize();
	camera.aspect = float(w) / h;

	camera.pos = {2.0f, 0.0f, 0.0f};
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

		if (window.getKey(glfw::KeyCode::Escape)) {
			window.setShouldClose(true);
		}

		Stopwatch timer;

		Update(delta_time);

		render_context.Clear();
		Render();
		render_context.Present();

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
	light.Render(camera);
	box.Render(camera, light);
	gui.Render();
}
