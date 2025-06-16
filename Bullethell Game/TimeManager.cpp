#include "TimeManager.h"

TimeManager::TimeManager(int _targetFps) {
	this->targetFps = _targetFps;
	this->currFps = targetFps;

	this->timePerFrame = 1.f / _targetFps;
	this->deltaTime = timePerFrame;
	this->beginClockMeasure = std::chrono::high_resolution_clock::now();
}

bool TimeManager::PushFrame() {

	//checks the difference between current and previous time values
	std::chrono::steady_clock::time_point currClockMeasure = std::chrono::high_resolution_clock::now();
	this->totalTimeElapsed = (float)std::chrono::duration_cast<std::chrono::nanoseconds>(currClockMeasure - beginClockMeasure).count() / 1000000000;

	this->frameTimeElapsed += totalTimeElapsed - previousTotalTimeElapsed;
	this->previousTotalTimeElapsed = totalTimeElapsed;

	//check if frame ready
	if (frameTimeElapsed >= timePerFrame) {
		this->deltaTime = frameTimeElapsed;

		this->currFps = 1.f / frameTimeElapsed;
		this->frameTimeElapsed -= timePerFrame;

		while (frameTimeElapsed - timePerFrame > 0)
			this->frameTimeElapsed -= timePerFrame;

		totalFrameCount++;
		return true;
	}
	else
		return false;
}

void TimeManager::SetFps(int _fps)
{
	this->targetFps = _fps;
	this->currFps = targetFps;

	this->timePerFrame = 1.f / _fps;
}

//time variable getters
float TimeManager::GetFps() const
{
	return this->currFps;
}

float TimeManager::GetTotalTime() const
{
	return this->totalTimeElapsed;
}

float TimeManager::GetDeltaTime() const
{
	//if (this->deltaTime < 1.f / this->currFps)
	//return 1.f / this->currFps;
	//else
		return this->deltaTime;
}
