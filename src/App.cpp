
#include "App.hpp"
#include "Utils.hpp"
#include "Stopwatch.hpp"

App::App(Window& window_): window(window_), camera(window)
{
	camera.pos = {2.0f, 0.0f, 0.0f};
}

App::~App()
{
	rhi::g_context->WaitIdle();
}

void App::Run()
{
	float delta_time = 0.0f;
	while (!window.ShouldClose())
	{
		Stopwatch timer;

		Update(delta_time);
		Render();

		delta_time = timer.GetFrameTime(gui.limit_framerate);
	}
}

void App::Update(float delta_time)
{
	window.Update(delta_time);
	camera.Update(delta_time);
	box.Update(delta_time);
	light.Update(delta_time);
	gui.Update(delta_time, light);
}

void App::Render()
{
	rhi::g_context->Clear();

	light.Render(camera);
	box.Render(camera, light);
	gui.Render();

	rhi::g_context->Present();
}
