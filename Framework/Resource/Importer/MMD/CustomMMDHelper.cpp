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

void PreProcessing_MMD_Vector3(Vector3& in, bool isPosition)
{
	//in.z *= -1;
	if (isPosition)
	{
		in *= 0.2f;
	}
}