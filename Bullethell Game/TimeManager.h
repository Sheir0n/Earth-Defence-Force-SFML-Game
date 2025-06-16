#pragma once
#include <chrono>
#include <iostream>
class TimeManager {
private:
	int targetFps;
	float timePerFrame;
	float deltaTime;
	float previousTotalTimeElapsed = 0.f;
	float frameTimeElapsed = 0.f;
	float totalTimeElapsed = 0.f;

	float currFps;
	int totalFrameCount = 0;

	std::chrono::steady_clock::time_point beginClockMeasure;

public:
	TimeManager(int _targetFps);

	bool PushFrame();

	void SetFps(int _fps);

	//time variable getters

	float GetFps() const;
	float GetTotalTime() const;
	float GetDeltaTime() const;
};