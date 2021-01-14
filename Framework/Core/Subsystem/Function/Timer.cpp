#include "Framework.h"
#include "Timer.h"


using namespace Framework;

Timer::Timer(Context* context)
	: ISubsystem(context)
	, deltaTimeMs(0.0f)
	, deltaTimeMs_Cumulated(0.0f)
	, deltaTimeMs_Return(0.0f)
	, deltaTime1Sec(0.0f)
{
}

bool Timer::Init()
{
	prevTime = std::chrono::high_resolution_clock::now();
	return true;
}

void Timer::Update()
{
	auto curTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms = curTime - prevTime;
	prevTime = curTime;

	deltaTimeMs = static_cast<float>(ms.count());

	deltaTimeMs_Cumulated += deltaTimeMs;

	if (deltaTimeMs_Cumulated > FPms)
	{
		isUpdatedFPS = true;

		deltaTimeMs_Return = deltaTimeMs_Cumulated; // important

		while (deltaTimeMs_Cumulated > FPms)
		{
			if (deltaTimeMs_Cumulated > 10000)
				deltaTimeMs_Cumulated -= 10000 * FPS;
			else
				deltaTimeMs_Cumulated -= FPS * 1000.f;
		}
		deltaNumUpdated += 1;
	}
	else
	{
		deltaTimeMs_Return = 0;
		isUpdatedFPS = false;
	}

	deltaTime1Sec += deltaTimeMs * 0.001f;

	if (deltaTime1Sec > 1.0f)
	{
		deltaTime1Sec = float(int(deltaTime1Sec) % 1);
		deltaNumUpdated = 0;
	}
}
