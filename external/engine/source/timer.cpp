#include <Timer.h>

void SDLTimer::Start()
{
	Started = true;
	Running = true;

	StartTicks = SDL_GetTicks();
	PauseTicks = 0;
}


void SDLTimer::Pause()
{
	if (Started && !Paused)
	{
		Running = false;
        Paused = true;
		PauseTicks = SDL_GetTicks() - StartTicks;
		StartTicks = 0;
	}
    else
    {
        Paused = false;
        Running = true;
		StartTicks = SDL_GetTicks() - PauseTicks;
		PauseTicks = 0;
    }
}

void SDLTimer::Stop()
{
	Started = false;
	Paused = false;

	StartTicks = 0;
	PauseTicks = 0;
}

bool SDLTimer::IsActive()
{
	return Started && Running;
}

uint32_t SDLTimer::GetTicks()
{
	if (Started)
	{
		if (Running)
		{
			return SDL_GetTicks() - StartTicks;
		}
		else
			return PauseTicks;
	}
	else
		return 0;
}