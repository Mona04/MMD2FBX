#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include <map>
#include "IResource.h"

namespace Framework
{
	struct Bone_Key
	{
		Bone_Key()
			: frame(-1), pos(0), scale(1), rot(0, 0, 0, 1)
			, bezier_x{ -1, -1 }, bezier_y{ -1, -1 }, bezier_z{ -1, -1 } { }
		Bone_Key(Vector3& pos, Vector3& scale, Quaternion& rot)
			: frame(-1), pos(pos), scale(scale), rot(rot)
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

	struct Bone_Channel
	{
		Bone_Channel() {}

		void LoadFromFile(class FileStream& stream);
		void SaveToFile(class FileStream& stream);

		std::vector<Bone_Key>& Get_Keys() { return keys; }
		// 시간 순서대로 넣어야함
		Bone_Key& Add_Key();
		const Bone_Key Get_Key(uint index) const;

		std::vector<Bone_Key> keys;  // frame / key 
	};

	struct Morph_Key   // memory 줄일려고 거의 같은 기능을 따로 뺀거
	{
		Morph_Key() : frame(-1), weight(0) {}
		Morph_Key(float weight, float frame)
			: weight(weight), frame(frame) {}

		void LoadFromFile(class FileStream& stream);
		void SaveToFile(class FileStream& stream);

		float weight;
		float frame;
	};

	struct Morph_Channel
	{
		Morph_Channel() {}

		void LoadFromFile(class FileStream& stream);
		void SaveToFile(class FileStream& stream);

		std::vector<Morph_Key>& Get_Keys() { return keys; }
		// 시간 순서대로 넣어야함
		Morph_Key& Add_Key();
		const Morph_Key Get_Key(uint index) const;

		std::vector<Morph_Key> keys;  // frame / key 
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
		void Init_Channels(int num) { _channels.clear(); _channels.resize(num); }
		std::vector<Bone_Channel>& Get_Channels() { return _channels; }
		Bone_Channel& Get_Channel(int i) { return _channels[i]; }

		void Init_Morph_Channels() { _morph_channels.clear(); }
		std::map<std::wstring, Morph_Channel>& Get_Morph_Channels() { return _morph_channels; }
		Morph_Channel& Get_Morph_Channel(const std::wstring& name) { return _morph_channels[name]; }

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

		std::vector<Bone_Channel> _channels;
		std::map<std::wstring, Morph_Channel> _morph_channels;

		bool _isMMD = false;
		bool _isLoop = true;
		bool _use_ik = true;
	};
}