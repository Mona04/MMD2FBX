#pragma once
#include <unordered_map>
#include "Core/Subsystem/ISubsystem.h"
#include "MMD2FBX.h"
#include "VMD2Binary.h"

namespace Framework
{
	class MMD2FBX_Converter : public ISubsystem
	{
	public:
		MMD2FBX_Converter(class Context* context);
		virtual ~MMD2FBX_Converter() {}

		virtual bool Init() override;
		virtual void Update() override;
		void Clear(bool not_frame);

		void Record();
		void Export();

		bool IsComplete() { return _is_complete; }
		void Set_Is_For_Binary(bool var) { _is_for_binary = var; }
		void SetSampleActor(const std::wstring& model_path, const std::wstring& anim_path);

		void Set_StartFrame(int var);
		void Set_EndFrame(int var) { _end_frame = var; }
		void Set_Ms_per_Tick(float var);

		float Get_Percentage() { return (float)(_cur_frame - _start_frame) / (float)(_end_frame - _start_frame); }

	protected:
		MMD2FBX _exporter;
		VMD2Binary _binary_exporter;

		class Actor* _actor = nullptr;
		bool _is_complete = false;
		bool _is_for_binary = false;

		int _start_frame = 0;
		int _end_frame = -1;
		int _cur_frame = 0;
		float _ms_per_tick = 30;
		std::wstring _current_anim_path;
	};
}