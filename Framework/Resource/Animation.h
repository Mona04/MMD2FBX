#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include "IResource.h"

namespace Framework
{
	struct Animation_Key
	{
		Animation_Key()
			: pos(0), scale(1), rot(0, 0, 0, 1), frame(-1)
			, bezier_x{ -1, -1 }, bezier_y{ -1, -1 }, bezier_z{ -1, -1 } {}
		Animation_Key(Vector3& pos, Vector3& scale, Quaternion& rot)
			: pos(pos), scale(scale), rot(rot), frame(-1)
			, bezier_x{ -1, -1 }, bezier_y{ -1, -1 }, bezier_z{ -1, -1 } {}

		void LoadFromFile(class FileStream& stream);
		void SaveToFile(class FileStream& stream);

		Vector3 pos;
		Vector3 scale;
		Quaternion rot;
		Vector2 bezier_x[2];
		Vector2 bezier_y[2];
		Vector2 bezier_z[2];
		Vector2 bezier_rot[2];
		float frame;
	};

	struct Animation_Channel
	{
		Animation_Channel(){}

		void LoadFromFile(class FileStream& stream);
		void SaveToFile(class FileStream& stream);

		std::vector<Animation_Key>& Get_Keys() { return keys; }
		// 시간 순서대로 넣어야함
		Animation_Key& Add_Key();
		const Animation_Key Get_Key(uint index) const;

		std::vector<Animation_Key> keys;  // frame / key 
	};

	class Animation : public IResource, public std::enable_shared_from_this<Animation>
	{
	public:
		Animation(class Context* context);
		virtual ~Animation();

		Animation(Animation& rhs) = delete;
		Animation(Animation&& rhs) = delete;
		Animation& operator=(Animation& rhs) = delete;
		Animation& operator=(Animation&& rhs) = delete;

		virtual bool LoadFromFile(std::wstring_view path) override;
		virtual bool SaveToFile(std::wstring_view path) override;
		virtual void Clear() override;

	public:
		// == Initializeing ===================================================

		// num : skeleton size. we makes relation between bone index and channel index;
		std::vector<Animation_Channel>& Init_Channels(int num) { _channels.clear(); _channels.resize(num); return _channels; }
		std::vector<Animation_Channel>& Get_Channels() { return _channels; }
		Animation_Channel& Get_Channel(int i) { return _channels[i];}

	
		void Set_Duration(float var) { _duration = var; }
		void Set_MsPerTic(float var) { _MsPerTic = var; }
		float Get_MsPerTic() { return _MsPerTic; }
		float Get_Duration() { return _duration; }
		void Set_IsLoop(bool var) { _isLoop = var; }
		bool IsLoop() { return _isLoop; }
		void Set_IsMMD(bool var) { _isMMD = var; }
		bool IsMMD() { return _isMMD; }
		void Set_UseIK(bool var) { _use_ik = var; }
		bool UseIK() { return _use_ik; }

		// =====================================================================	

	private:
		float _MsPerTic = 0;
		float _duration = 0;
		
		std::vector<Animation_Channel> _channels;
		bool _isMMD = false;
		bool _isLoop = true;
		bool _use_ik = true;
	};
}