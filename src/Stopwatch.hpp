
#pragma once

#include <chrono>
#include <thread>

struct Stopwatch {
	static std::chrono::high_resolution_clock::time_point Now() {
		return std::chrono::high_resolution_clock::now();
	}
	std::chrono::high_resolution_clock::time_point start;

	Stopwatch() {
		start = Now();
	}

	using Seconds = std::chrono::duration<float>;

	float GetFrameTime(bool limit_framerate) {
		if (limit_framerate) {
			auto finish = Now();
			auto frame_time = finish - start;
			const int target_fps = 60;
			auto target_frame_time = Seconds(1.0f) / target_fps;
			if (frame_time < target_frame_time) {
				auto sleep_time = std::chrono::duration_cast<std::chrono::milliseconds>(target_frame_time - frame_time);
				std::this_thread::sleep_for(sleep_time);
			}
		}
		auto finish = Now();
		return std::chrono::duration_cast<Seconds>(finish - start).count();
	}
};
