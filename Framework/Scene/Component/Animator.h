#pragma once
#include "IComponent.h"

namespace Framework
{
	class Animator : public IComponent
	{
	public:
		Animator(class Context* context);
		virtual ~Animator();

		virtual bool Update() override;
		virtual void Clear() override;

		void SetAnimation(std::wstring_view path);
		std::shared_ptr<Animation> GetAnimation() { return _animation; }

		void Play() { _isPlaying = true; }
		void Stop() { _isPlaying = false; _current_frame = 0; _isEnd = false; }
		void Pause() { _isPlaying = false; }

		bool IsEnd() { return _isEnd; }
		void SetMsPerTick(float var) { _ms_per_tick = var; }
		void SetCurrentFrame(uint var) { _current_frame = var; }

	protected:
		void Animate();

		struct Animation_Key Get_KeyFrame_Matrix(int index);
		std::pair<uint, uint> Update_Key_Index(uint frame_index, const struct Animation_Channel& channel);
		struct Animation_Key Interporate_Key(const Animation_Key& before, const Animation_Key& after);


	protected:
		class Timer* _timer;
		class Transform* _root;
		std::shared_ptr<class Animation> _animation;		

		bool _isPlaying = false;
		bool _isEnd = false;
		float _current_accumulated_delta_time_ms = 0;
		uint _current_frame = 0;
		std::vector<uint> _current_key_index_map; // channel index / key index

		float _ms_per_tick = 30.f; // when animate on real time, we don need this term but when converting it need.
	};
}