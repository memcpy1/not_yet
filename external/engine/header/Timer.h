#pragma once
#include <cstdint>
#include <chrono>
#include <ctime>
#include <iostream>
//SDL
#include <SDL.h>

class SDLTimer
{
private:
	uint32_t StartTicks;
	uint32_t PauseTicks;
	
	uint32_t LastTick;
	uint32_t CurrentTick;

	bool Started;
    bool Paused;
	bool Running;
public:
	SDLTimer()
		: StartTicks(0), PauseTicks(0), LastTick(0), CurrentTick(SDL_GetTicks()),
	Started(0), Paused(0), Running(0) {}

	void Start();
	void Pause();	
	void Stop();

	bool IsActive();
	uint32_t GetTicks();
};