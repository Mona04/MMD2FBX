#pragma once
#include "../ISubsystem.h"
#include <chrono>

namespace Framework
{
	class Timer final : public ISubsystem
	{
	public:
		Timer(class Context* context);
		~Timer() = default;

		bool Init() override;

		const float& GetDeltaTimeMs() const { return deltaTimeMs_Return; }

		void Update();

		// manage tick by updating when IsUpdatedFPS is true
		const bool& IsUpdatedFPS() { return isUpdatedFPS; }

	private:
		float deltaTimeMs;

		// just for calculate in class
		float deltaTimeMs_Cumulated;

		// to return delta time
		float deltaTimeMs_Return;

		// to calc FPS
		float deltaTime1Sec;
		int deltaNumUpdated;

		bool isUpdatedFPS;

		std::chrono::high_resolution_clock::time_point prevTime;

		const float FPS = 1.0f / 60.0f;
		const float FPms = 1000.f / 60.0f;
	};
}