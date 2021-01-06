#pragma once

namespace Framework
{
	class Context;
	class Actor;
	class Context;
	class Mesh;
	class Material;
	class Transform;
	class Animator;
	struct Animation_Key;
	struct Animation_Channel;
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
	std::vector<Framework::Animation_Channel> _channels_data;
	Framework::Actor* _actor;

	float _ms_per_tick = -1;
	int _start_frame = 0;
	int _end_frame = -1;
};