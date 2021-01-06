#include "Framework.h"
#include "Animation.h"
#include <algorithm>

using namespace Framework;

void Animation_Key::LoadFromFile(FileStream& stream)
{
	stream.Read(pos);
	stream.Read(scale);
	stream.Read(rot);
	stream.Read(bezier_x[0]);   stream.Read(bezier_x[1]);
	stream.Read(bezier_y[0]);   stream.Read(bezier_y[1]);
	stream.Read(bezier_z[0]);   stream.Read(bezier_z[1]);
	stream.Read(bezier_rot[0]); stream.Read(bezier_rot[1]);
	stream.Read(frame);
}

void Animation_Key::SaveToFile(FileStream& stream)
{
	stream.Write(pos);
	stream.Write(scale);
	stream.Write(rot);
	stream.Write(bezier_x[0]);   stream.Write(bezier_x[1]);
	stream.Write(bezier_y[0]);   stream.Write(bezier_y[1]);
	stream.Write(bezier_z[0]);   stream.Write(bezier_z[1]);
	stream.Write(bezier_rot[0]); stream.Write(bezier_rot[1]);
	stream.Write(frame);
}

void Animation_Channel::LoadFromFile(FileStream& stream)
{
	uint nKeys = 0;
	stream.Read(nKeys);
	keys.resize(nKeys);

	for (auto& key : keys)
		key.LoadFromFile(stream);
}

void Animation_Channel::SaveToFile(FileStream& stream)
{
	uint nKeys = static_cast<uint>(keys.size());
	stream.Write(nKeys);

	for (auto& key : keys)
		key.SaveToFile(stream);
}

Animation_Key& Animation_Channel::Add_Key()
{
	return keys.emplace_back(Animation_Key());
}

const Animation_Key Animation_Channel::Get_Key(uint index) const
{
	if (keys.size() <= index) 
		return Animation_Key();

	return keys[index];
}

Animation::Animation(Context* context) : IResource(context)
{
	_typecode = TypeCode::Animation;

	Clear();
}

Animation::~Animation()
{
}

bool Animation::LoadFromFile(std::wstring_view path)
{
	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Read);
	{
		stream.Read(_MsPerTic);
		stream.Read(_duration);
		stream.Read(_isMMD);
		stream.Read(_isLoop);
		stream.Read(_use_ik);

		uint nChannels;
		stream.Read(nChannels);
		_channels.resize(nChannels);

		for (auto& channel : _channels)
			channel.LoadFromFile(stream);
	}
	return false;
}

bool Animation::SaveToFile(std::wstring_view path)
{
	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Write);
	{
		stream.Write(_MsPerTic);
		stream.Write(_duration);
		stream.Write(_isMMD);
		stream.Write(_isLoop);
		stream.Write(_use_ik);
		stream.Write(static_cast<uint>(_channels.size()));

		for (auto& channel : _channels)
			channel.SaveToFile(stream);
	}
	return false;
}

void Animation::Clear()
{
}