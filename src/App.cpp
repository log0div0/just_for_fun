
#include "App.hpp"
#include "Utils.hpp"
#include <math/vector.hpp>
#include <iostream>
#include <thread>
#include <scope_guard.hpp>

using namespace math::literals;
using namespace std::chrono_literals;

App::App(glfw::Window& window_): window(window_), camera(window), gui(window)
{
	int w = 1200;
	int h = 900;

	window.setSize(w, h);

	window.framebufferSizeEvent.setCallback([&](glfw::Window& window, int w, int h) {
		rhi::SetViewportSize(w, h);
		camera.aspect = float(w) / h;
	});
	rhi::SetViewportSize(w, h);
	camera.aspect = float(w) / h;

	camera.pos = {2.0f, 0.0f, 0.0f};
}

App::~App()
{
}

using Seconds = std::chrono::duration<float>;
auto Now() {
	return std::chrono::high_resolution_clock::now();
}

void App::Run()
{
	Seconds delta_time(0.0f);
	while (!window.shouldClose())
	{
		if (window.getKey(glfw::KeyCode::Escape)) {
			window.setShouldClose(true);
		}

		auto start = Now();

		Update(delta_time.count());
		Render();

		window.swapBuffers();
		glfw::pollEvents();

		if (gui.limit_framerate) {
			auto finish = Now();
			auto frame_time = finish - start;
			const int target_fps = 60;
			auto target_frame_time = Seconds(1.0f) / target_fps;
			if (frame_time < target_frame_time) {
				auto sleep_time = std::chrono::duration_cast<std::chrono::milliseconds>(target_frame_time - frame_time);
				std::this_thread::sleep_for(sleep_time);
			}
		}
		auto now = Now();
		delta_time = std::chrono::duration_cast<Seconds>(now - start);
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
	rhi::ClearViewport();

	light.Render(camera);
	box.Render(camera, light);
	gui.Render();
}
