#include "Framework.h"
#include "FileStream.h"
#include "FileSystem.h"

using namespace Framework;

bool FileStream::Open(const std::string& path, const StreamMode& mode)
{
	this->mode = mode;

	switch (mode)
	{
	case StreamMode::Write:
		out.open(path, std::ios::out | std::ios::binary);
		if (out.fail())
		{
			LOG_ERROR("Fail To Open \"%s\" for Writing", path.c_str());
			return false;
		}
		break;
	case StreamMode::Read:
		if (FileSystem::IsExistFile(path) == false)
			return false;
		in.open(path, std::ios::in | std::ios::binary);
		if (in.fail())
		{
			LOG_ERROR("Fail To Open \"%s\" for Reading", path.c_str());
			return false;
		}		
		break;
	case StreamMode::AsciiWrite:
		out.open(path, std::ios::out);
		if (out.fail())
		{
			LOG_ERROR("Fail To Open \"%s\" for Writing", path.c_str());
			return false;
		}
		break;
	case StreamMode::AscilRead:
		if (FileSystem::IsExistFile(path) == false)
			return false;
		in.open(path, std::ios::in);
		if (in.fail())
		{
			LOG_ERROR("Fail To Open \"%s\" for Reading", path.c_str());
			return false;
		}
		break;
	}

	return true;
}

bool FileStream::Open(const std::wstring& path, const StreamMode& mode)
{
	this->mode = mode;

	switch (mode)
	{
	case StreamMode::Write:
		out.open(path, std::ios::out | std::ios::binary);
		if (out.fail())
		{
			LOG_ERROR("Fail To Open \"%s\" for Writing", path.c_str());
			return false;
		}
		break;
	case StreamMode::Read:
		if (FileSystem::IsExistFile(path) == false)
			return false;
		in.open(path, std::ios::in | std::ios::binary);
		if (in.fail())
		{
			LOG_ERROR("Fail To Open \"%s\" for Reading", path.c_str());
			return false;
		}		
		break;
	case StreamMode::AsciiWrite:
		out.open(path, std::ios::out);
		if (out.fail())
		{
			LOG_ERROR("Fail To Open \"%s\" for Writing", path.c_str());
			return false;
		}
		break;
	case StreamMode::AscilRead:
		if (FileSystem::IsExistFile(path) == false)
			return false;
		in.open(path, std::ios::in);
		if (in.fail())
		{
			LOG_ERROR("Fail To Open \"%s\" for Reading", path.c_str());
			return false;
		}
		break;
	}



	return true;
}

void FileStream::Close()
{
	switch (mode)
	{
	case StreamMode::Write:
		out.flush();
		out.close();
		break;

	case StreamMode::Read:
		in.clear();
		in.close();
		break;
	}
}

void FileStream::Write(const std::string& value)
{
	uint length = static_cast<uint>(value.length());
	Write(length);

	out.write(value.c_str(), length);
}

void FileStream::Write(const std::wstring& value)
{
	uint length = static_cast<uint>(value.length());
	Write(length);

	for (int i = 0; i < length; i++)
		out.write(reinterpret_cast<const char*>(&value[i]), sizeof(WCHAR));
}


void FileStream::Write(const std::vector<std::byte>& value)
{
	uint length = static_cast<uint>(value.size());
	Write(length);

	out.write(reinterpret_cast<const char*>(value.data()), sizeof(std::byte) * length);
}

void FileStream::Read(std::string& value)
{
	uint length = ReadUInt();

	value.clear();
	value.shrink_to_fit();
	value.resize(length);

	in.read(value.data(), length);
}

void FileStream::Read(std::wstring& value)
{
	uint length = ReadUInt();
	WCHAR _wchar;
	value.clear();
	value.shrink_to_fit();
	value.resize(length);

	for (int i = 0; i < length; i++)
	{
		in.read(reinterpret_cast<char*>(&_wchar), sizeof(WCHAR));
		value[i] = _wchar;
	}
}

void FileStream::Read(std::vector<std::byte>& value)
{
	uint length = ReadUInt();

	value.clear();
	value.shrink_to_fit();
	value.reserve(length);
	value.resize(length);

	in.read(reinterpret_cast<char*>(value.data()), sizeof(std::byte) * length);
}

const uint FileStream::ReadUInt()
{
	uint value = 0;
	Read(value);

	return value;
}
