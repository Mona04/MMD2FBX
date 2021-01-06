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
		const float& GetDeltaTimeSec() const { return deltaTimeSec_Return; }

		void Update();

		// manage tick by updating when IsUpdatedFPS is true
		const bool& IsUpdatedFPS() { return isUpdatedFPS; }

	private:
		float deltaTimeMs;
		float deltaTimeSec;

		// just for calculate in class
		float deltaTimeMs_Cumulated;
		float deltaTimeSec_Cumulated;

		// for return delta time
		float deltaTimeMs_Return;
		float deltaTimeSec_Return;

		float deltaTime1Sec;
		int deltaNumUpdated;

		bool isUpdatedFPS;

		std::chrono::high_resolution_clock::time_point prevTime;
	};
}