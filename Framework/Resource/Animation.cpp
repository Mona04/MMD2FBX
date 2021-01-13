#include "Framework.h"
#include "Animation.h"
#include <algorithm>

using namespace Framework;


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
	Clear();

	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Read);
	{
		stream.Read(_MsPerTic);
		stream.Read(_duration);
		stream.Read(_isMMD);
		stream.Read(_isLoop);
		stream.Read(_use_ik);

		uint nChannels = stream.ReadUInt();
		_channels.resize(nChannels);

		for (auto& channel : _channels)
			channel.LoadFromFile(stream);

		nChannels = stream.ReadUInt();
		for (int i = 0; i < nChannels; i++)
		{
			std::wstring name;
			stream.Read(name);
			_morph_channels[name].LoadFromFile(stream);
		}
	}

	return true;
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

		stream.Write(static_cast<uint>(_morph_channels.size()));
		for (auto& pair : _morph_channels)
		{
			stream.Write(pair.first);
			pair.second.SaveToFile(stream);
		}
	}

	return true;
}

void Animation::Clear()
{
	_MsPerTic = 0;
	_duration = 0;

	_channels.clear();
	_channels.shrink_to_fit();

	_morph_channels.clear();

	_isMMD = false;
	_isLoop = true;
	_use_ik = true;
}



// =============== Frame Datas ==============================================

void Bone_Key::LoadFromFile(FileStream& stream)
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

void Bone_Key::SaveToFile(FileStream& stream)
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

void Bone_Channel::LoadFromFile(FileStream& stream)
{
	uint nKeys = 0;
	stream.Read(nKeys);
	keys.resize(nKeys);

	for (auto& key : keys)
		key.LoadFromFile(stream);
}

void Bone_Channel::SaveToFile(FileStream& stream)
{
	uint nKeys = static_cast<uint>(keys.size());
	stream.Write(nKeys);

	for (auto& key : keys)
		key.SaveToFile(stream);
}

Bone_Key& Bone_Channel::Add_Key()
{
	return keys.emplace_back(Bone_Key());
}

const Bone_Key Bone_Channel::Get_Key(uint index) const
{
	if (keys.size() <= index)
		return Bone_Key();

	return keys[index];
}


void Morph_Key::LoadFromFile(FileStream& stream)
{
	stream.Read(weight);
	stream.Read(frame);
}

void Morph_Key::SaveToFile(FileStream& stream)
{
	stream.Write(weight);
	stream.Write(frame);
}

void Morph_Channel::LoadFromFile(FileStream& stream)
{
	uint nKeys = 0;
	stream.Read(nKeys);
	keys.resize(nKeys);

	for (auto& key : keys)
		key.LoadFromFile(stream);
}

void Morph_Channel::SaveToFile(FileStream& stream)
{
	uint nKeys = static_cast<uint>(keys.size());
	stream.Write(nKeys);

	for (auto& key : keys)
		key.SaveToFile(stream);
}

Morph_Key& Morph_Channel::Add_Key()
{
	return keys.emplace_back(Morph_Key());
}

const Morph_Key Morph_Channel::Get_Key(uint index) const
{
	if (keys.size() <= index)
		return Morph_Key();

	return keys[index];
}
