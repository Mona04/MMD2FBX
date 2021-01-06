#include "Framework.h"
#include "Timer.h"

#define FPS 1.0f/61.0f
#define RFPS 61.0f

using namespace Framework;

Timer::Timer(Context* context)
	: ISubsystem(context)
	, deltaTimeMs(0.0f)
	, deltaTimeSec(0.0f)
	, deltaTimeMs_Cumulated(0.0f)
	, deltaTimeSec_Cumulated(0.0f)
	, deltaTimeSec_Return(0.0f)
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
	deltaTimeSec = static_cast<float>(ms.count() * 0.001);

	deltaTimeMs_Cumulated += deltaTimeMs;
	deltaTimeSec_Cumulated += deltaTimeSec;

	if (deltaTimeSec_Cumulated > FPS)
	{
		isUpdatedFPS = true;

		deltaTimeMs_Return = deltaTimeMs_Cumulated;
		deltaTimeSec_Return = deltaTimeSec_Cumulated;

		deltaTimeSec_Cumulated = 0;
		deltaTimeMs_Cumulated = 0;
		deltaNumUpdated += 1;

		if (deltaNumUpdated > RFPS)
		{
			deltaTimeSec_Cumulated = 0;// int(deltaNumUpdated) % int(RFPS);
			deltaTimeMs_Cumulated = 0;//int(deltaNumUpdated) % int(RFPS);
		}
	}
	else
	{
		isUpdatedFPS = false;
	}

	deltaTime1Sec += deltaTimeSec;
	
	if (deltaTime1Sec > 1.0f)
	{
		deltaTime1Sec = float(int(deltaTime1Sec) % 1);
		deltaNumUpdated = 0;
	}
}
