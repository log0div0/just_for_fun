
#include "App.hpp"
#include "Utils.hpp"
#include <math/vector.hpp>
#include <iostream>
#include <scope_guard.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace math::literals;

App::App(glfw::Window window_): window(std::move(window_))
{
	InitWindow();
	InitRenderer();
	InitInput();
	InitWorld();
	InitGUI();
}

App::~App()
{
	DeinitGUI();
}

void App::InitGUI() {
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");
}

void App::DeinitGUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void App::InitWindow() {
	int w = 1200;
	int h = 900;

	window.setSize(w, h);

	window.framebufferSizeEvent.setCallback([&](glfw::Window& window, int w, int h) {
		glViewport(0, 0, w, h);
		camera.aspect = float(w) / h;
	});
	glViewport(0, 0, w, h);
	camera.aspect = float(w) / h;
}

void App::InitRenderer() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glClearDepth(0.0);
}

void App::InitInput() {
	last_cursor_pos = std::make_from_tuple<CursorPos>(window.getCursorPos());
	window.cursorPosEvent.setCallback([&](glfw::Window& window, double x, double y) {
		CursorPos pos{x, y};
		if (window.getMouseButton(glfw::MouseButton::Right)) {
			CursorPos diff = last_cursor_pos - pos;
			camera.OnCursorMove(diff.X(), diff.Y());
		}
		last_cursor_pos = pos;
	});
	window.scrollEvent.setCallback([&](glfw::Window& window, double x, double y) {
		camera.OnScroll(y);
	});
}

void App::InitWorld() {
	camera.pos = {2.0f, 0.0f, 0.0f};
}

void App::Run()
{
	float delta_time = 0.0f;
	while (!window.shouldClose())
	{
		double start = glfwGetTime();

		if (delta_time) {
			ProcessInput(delta_time);
			UpdateWorld(delta_time);
		}

		Render();

		window.swapBuffers();
		glfw::pollEvents();

		while (true) {
			double finish = glfwGetTime();

			double frame_time = finish - start;
			const int target_fps = 60;
			double target_frame_time = 1.0 / target_fps;

			if (frame_time > target_frame_time) {
				delta_time = finish - start;
				break;
			}
			glfw::waitEvents(target_frame_time - frame_time);
		}
	}
}

void App::ProcessInput(float delta_time)
{
	if (window.getKey(glfw::KeyCode::Escape)) {
		window.setShouldClose(true);
	}
	if (window.getKey(glfw::KeyCode::W)) {
		camera.MoveForward(delta_time);
	}
	if (window.getKey(glfw::KeyCode::S)) {
		camera.MoveBack(delta_time);
	}
	if (window.getKey(glfw::KeyCode::D)) {
		camera.MoveRight(delta_time);
	}
	if (window.getKey(glfw::KeyCode::A)) {
		camera.MoveLeft(delta_time);
	}
	if (window.getKey(glfw::KeyCode::E)) {
		camera.MoveUp(delta_time);
	}
	if (window.getKey(glfw::KeyCode::Q)) {
		camera.MoveDown(delta_time);
	}
}

void App::UpdateWorld(float delta_time)
{
	box.Update(delta_time);
}

void App::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	box.Render(camera);
}
