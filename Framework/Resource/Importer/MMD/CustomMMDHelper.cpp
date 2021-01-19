#include "Framework.h"
#include "CustomMMDHelper.h"
#include "Util/FileSystem.h"

std::wstring ReadPmxWString(std::istream* _stream, int size)
{
	char buffer[30];
	
	_stream->read(buffer, size);
	
	for (int i = 0; i < size; i++)
	{
		if (buffer[i] == '\0')
		{
			size = i - 1;
			break;
		}
	}
	
	if (size > 0)
	{
		return Framework::FileSystem::ToWString(buffer, true);
	}
	else
	return std::wstring(L"N/A");
}

std::string ReadPmxString(std::istream* _stream, int size)
{
	char buffer[30];

	_stream->read(buffer, size);

	for (int i = 0; i < size; i++)
	{
		if (buffer[i] == '\0')
		{
			size = i - 1;
			break;
		}
	}

	return std::string(buffer);
}

void PreProcess_MMD_Vector3(Vector3& in, bool bScale)
{
	if (bScale)
	{
		//in *= 0.2f;
	}
}

Vector3 PreProcess_MMD_Vector3(float x, float y, float z, bool bScale)
{
	Vector3 result(x, y, z);
	//if(bScale)
	//	result *= 0.2f;
	return result;
}
