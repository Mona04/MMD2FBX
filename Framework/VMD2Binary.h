#pragma once
#include <vector>
#include <map>

namespace Framework
{
	class Context;
	class Actor;
	class Context;
	class Mesh;
	class Material;
	class Transform;
	class Animator;
	struct Bone_Key;
	struct Bone_Channel;
	struct Morph_Channel;
}

class VMD2Binary
{
public:
	VMD2Binary();
	~VMD2Binary();

	bool Init(Framework::Actor* actor, int frame_start = 0, int frame_end = -1);
	void Clear();

	bool Record_Animation_Frame(int cur_tik);
	bool Export(const std::wstring_view pFile, Framework::Context* context);

private:
	std::vector<Framework::Bone_Channel> _channels_data;
	std::map<std::wstring, Framework::Morph_Channel> _morph_channels_data;
	Framework::Actor* _actor;

	float _ms_per_tick = -1;
	int _start_frame = 0;
	int _end_frame = -1;
};