#include "Framework.h"
#include "FileSystem.h"
#include <filesystem>
#include <ShlObj.h>

using namespace Framework;

void FileSystem::OpenFilePathDialogA(std::function<void(std::string)>&& function, const char* filter, const char* directory, HWND hwnd)
{
	ITEMIDLIST* pidlBrowse;
	char buffer[255];
	WCHAR displayName[255] = L"## Open ##";
	WCHAR title[255] = L"Path Search";

	BROWSEINFOW BrInfo;
	memset(&BrInfo, 0, sizeof(BrInfo));
	BrInfo.hwndOwner = hwnd;
	BrInfo.pidlRoot = NULL;
	BrInfo.pszDisplayName = displayName;
	BrInfo.lpszTitle = title;
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	pidlBrowse = SHBrowseForFolderW(&BrInfo);

	if (pidlBrowse != NULL)
	{
		if (TRUE == SHGetPathFromIDListA(pidlBrowse, buffer))
		{
			std::string path = GetSlashPath(std::string(buffer));
			std::function<void(std::string)> func =
				std::forward<std::function<void(std::string)>>(function);

			if (!function)
				func(path);
		}
	}
}

void FileSystem::OpenFilePathDialogW(std::function<void(std::wstring)>&& function, const WCHAR* filter, const WCHAR* directory, HWND hwnd)
{
	ITEMIDLIST* pidlBrowse;
	WCHAR buffer[255];
	WCHAR displayName[255] = L"## Unicode Disable ##";
	WCHAR title[255] = L"Path Search";

	BROWSEINFOW BrInfo;
	memset(&BrInfo, 0, sizeof(BrInfo));
	BrInfo.hwndOwner = hwnd;
	BrInfo.pidlRoot = NULL;
	BrInfo.pszDisplayName = displayName;
	BrInfo.lpszTitle = title;

	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// ´ÙÀÌ¾ó·Î±× ¶ç¿ì±â
	pidlBrowse = SHBrowseForFolderW(&BrInfo);

	if (pidlBrowse != NULL)
	{
		if (TRUE == ::SHGetPathFromIDListW(pidlBrowse, buffer))
		{
			std::wstring path = GetSlashPath(std::wstring(buffer));
			std::function<void(std::wstring)> func =
				std::forward<std::function<void(std::wstring)>>(function);

			if (!function)
				func(path);
		}
	}
}

void FileSystem::OpenFileDialogA(std::function<void(std::string)>&& function, const char* filter, const char* directory, HWND hwnd)
{
	char buffer[255];
	ZeroMemory(buffer, 255);

	char initialPath[255];
	ZeroMemory(initialPath, 255);
	_fullpath(initialPath, directory, 255);

	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buffer;
	ofn.lpstrInitialDir = initialPath;
	ofn.nMaxFile = 255;
	ofn.Flags = OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE) {

		std::string path = std::string(buffer);
		std::function<void(std::string)> func =
			std::forward<std::function<void(std::string)>>(function);

		if (!function)
			func(path);
	}
}

void FileSystem::OpenFileDialogW(std::function<void(std::wstring)>&& function, const WCHAR* filter, const WCHAR* directory, HWND hwnd)
{
	WCHAR buffer[255];
	ZeroMemory(buffer, 255);

	WCHAR initialPath[255];
	ZeroMemory(initialPath, 255);
	_wfullpath(initialPath, directory, 255);

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buffer;
	ofn.lpstrInitialDir = initialPath;
	ofn.nMaxFile = 255;
	ofn.Flags = OFN_NOCHANGEDIR;

	if (GetOpenFileNameW(&ofn) == TRUE) {

		std::wstring path = std::wstring(buffer);
		std::function<void(std::wstring)> func =
			std::forward<std::function<void(std::wstring)>>(function);

		if (!function)
			func(path);
	}
}

void FileSystem::CreateFolder(const std::string& path)
{
	if (FALSE == CreateDirectoryA(path.c_str(), NULL))
	{
	}
}

void FileSystem::CreateFolder(const std::wstring& path)
{
	if (FALSE == CreateDirectoryW(path.c_str(), NULL))
	{
	}
}

const std::string FileSystem::GetWorkingDirectoryA()
{
	return std::filesystem::current_path().generic_string() + "/";
}

const std::wstring FileSystem::GetWorkingDirectoryW()
{
	return std::filesystem::current_path().generic_wstring() + L"/";
}

bool FileSystem::IsExistFile(std::string_view path)
{
	try {
		return std::filesystem::exists(path);
	}
	catch (std::filesystem::filesystem_error error)
	{
		LOG_WARNING("Failed to searching str");
	}
	return false;
}

bool FileSystem::IsExistFile(std::wstring_view path)
{
	try {
		return std::filesystem::exists(path);
	}
	catch (std::filesystem::filesystem_error error)
	{
		LOG_WARNING("Failed to searching str");
	}
	return false;
}

std::vector<std::wstring> FileSystem::GetFilesInDirectory(const std::wstring& path)
{
	std::vector<std::wstring> files;

	std::filesystem::directory_iterator endIter;
	for (std::filesystem::directory_iterator iter(path); iter != endIter; iter++)
	{
		if (!std::filesystem::is_regular_file(iter->status()))
			continue;
		files.emplace_back(iter->path().generic_wstring());
	}

	return files;
}


// ========================================================================
// STD String Convert
// ========================================================================
// buffer size is 1024
std::wstring FileSystem::ToWString(std::string_view str, bool isJapanese)
{
	wchar_t buffer[1024];

	DWORD minSize;
	minSize = MultiByteToWideChar(isJapanese ? 932 : CP_ACP, 0, str.data(), -1, NULL, 0);

	if (1024 < minSize)
		return L"Size over";

	// Convert string from multi-byte to Unicode.
	MultiByteToWideChar(isJapanese ? 932 : CP_ACP, 0, str.data(), -1, buffer, minSize);

	return std::wstring(buffer);
}

// buffer size is 1024
std::string FileSystem::ToString(std::wstring_view str, bool isJapanese)
{
	char buffer[1024];

	DWORD minSize;	
	minSize = WideCharToMultiByte(isJapanese ? 932 : CP_ACP, 0, str.data(), -1, NULL, 0, 0, 0);

	if (1024 < minSize)
		return "Size over";

	// Convert string from multi-byte to Unicode.
	WideCharToMultiByte(isJapanese ? 932 : CP_ACP, 0, str.data(), -1, buffer, minSize, 0, 0);

	return std::string(buffer);
}


// =======================================================================================
//   STRING
// =======================================================================================

std::string FileSystem::ToAbsolute(std::string_view str)
{
	return std::filesystem::absolute(str).generic_string();
}

std::string FileSystem::ToSmaller(std::string_view str)
{
	std::string result(str);
	for (auto i = result.begin(); i < result.end(); i++)
	{
		if (*i <= 'Z' && *i >= 'A')
			*i = *i - ('Z' - 'z');
	}
	return result;;
}

std::string FileSystem::Find_Replace_All(std::string_view str, std::string_view oldStr, std::string_view newStr)
{
	std::string result(str);
	std::string::size_type pos = 0u;
	while ((pos = str.find(oldStr, pos)) != std::string::npos) {
		result.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
	return result;
}

// return like "asfsf/sdfasff/", 
std::string FileSystem::GetFileDirectoryFromPath(std::string_view path)
{
	auto lastIndex = path.find_last_of("\\/");
	auto fileName = path.substr(0, lastIndex+1); // +1 안하면 끝에 \\ 이거 안나옴

	return std::string(fileName);
}

// Automatically return not Upper case but Smaller Case without comma
std::string FileSystem::GetFileExtensionFromPath(std::string_view path) 
{
	auto fileName = GetFileNameFromPath(path);
	auto lastIndex = fileName.find_last_of('.');
	auto extention = fileName.substr(lastIndex + 1, fileName.length());

	return ToSmaller(extention);
}

std::string FileSystem::GetFileNameFromPath(std::string_view path)
{
	auto lastIndex = path.find_last_of("\\/");
	auto fileName = path.substr(lastIndex + 1, path.length());

	return std::string(fileName);
}

std::string FileSystem::GetIntactFileNameFromPath(std::string_view path)
{
	auto fileName = GetFileNameFromPath(path);
	auto lastIndex = fileName.find_last_of('.');
	auto intactFileName = fileName.substr(0, lastIndex);

	return intactFileName;
}

std::string FileSystem::GetSlashPath(const std::string& path)
{
	char result[1024];
	std::string from = "\\";
	std::string to = "//";

	int i = 0;
	for (auto ch : path)
	{
		if (ch == '\\')
		{
			result[i++] = '/';
			result[i++] = '/';
		}
		else result[i++] = ch;
	}

	result[i++] = '\0';

	return std::string(result);
}

std::string FileSystem::GetRelativeFilePath(const std::string& path)
{
	std::filesystem::path p = std::filesystem::absolute(path);
	std::filesystem::path r = std::filesystem::absolute(GetWorkingDirectoryA());

	if (p.root_path() != r.root_path())
		return p.generic_string();

	std::filesystem::path result;

	std::filesystem::path::const_iterator iter_p = p.begin();
	std::filesystem::path::const_iterator iter_r = r.begin();

	while (*iter_p == *iter_r && iter_p != p.end() && iter_r != r.end()) {
		iter_p++;
		iter_r++;
	}

	if (iter_r != r.end())
	{
		iter_r++;
		while (iter_r != r.end())
		{
			result /= "..";
			iter_r++;
		}
	}

	while (iter_p != p.end())
	{
		result /= *iter_p;
		iter_p++;
	}

	return result.generic_string();
}

//Script 쓰려고 만든거. Parted = Actor_Context, Dividig = _  => return {Actor, Context)
std::vector<std::string> FileSystem::GetPartsOfString(std::string_view parted, std::string_view dividing)
{
	std::vector<std::string> names;
	std::string pureName = GetIntactFileNameFromPath(parted);

	size_t lastIndex = pureName.find_last_of(dividing);
	std::string name = pureName.substr(0, lastIndex);
	std::string arg = pureName.substr(lastIndex + dividing.size(), pureName.length());

	names.emplace_back(name); names.emplace_back(arg);

	return names;
}


// =======================================================================================
//   WSTRINg
// =======================================================================================

std::wstring FileSystem::ToAbsolute(std::wstring_view str)
{
	return std::filesystem::absolute(str).generic_wstring();
}

std::wstring FileSystem::ToSmaller(std::wstring_view str)
{
	std::wstring result(str);
	for (auto i = result.begin(); i < result.end(); i++)
	{
		if (*i <= 'Z' && *i >= 'A')
			*i = *i - ('Z' - 'z');
	}
	return result;;
}

std::wstring FileSystem::Find_Replace_All(std::wstring_view str, std::wstring_view oldStr, std::wstring_view newStr)
{
	std::wstring result(str);
	std::wstring::size_type pos = 0u;
	while ((pos = str.find(oldStr, pos)) != std::wstring::npos) {
		result.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
	return result;
}

std::wstring FileSystem::GetFileDirectoryFromPath(std::wstring_view path)
{
	auto lastIndex = path.find_last_of(L"\\/");
	auto fileName = path.substr(0, lastIndex + 1); // +1 안하면 끝에 \\ 이거 안나옴

	return std::wstring(fileName);
}

// .txt 형식임
std::wstring FileSystem::GetFileExtensionFromPath(std::wstring_view path)
{
	auto fileName = GetFileNameFromPath(path);
	auto lastIndex = fileName.find_last_of('.');
	auto extention = fileName.substr(lastIndex, fileName.length());

	return ToSmaller(extention);
}

std::wstring FileSystem::GetFileNameFromPath(std::wstring_view path)
{
	auto lastIndex = path.find_last_of(L"\\/");
	auto fileName = path.substr(lastIndex + 1, path.length());

	return std::wstring(fileName);
}

std::wstring FileSystem::GetIntactFileNameFromPath(std::wstring_view path)
{
	auto fileName = GetFileNameFromPath(path);
	auto lastIndex = fileName.find_last_of('.');
	auto intactFileName = fileName.substr(0, lastIndex);

	return intactFileName;
}

std::wstring FileSystem::GetPartPath(std::wstring_view str, int num_parent)
{
	if (str == None_StringW)
		return None_StringW;
	std::wstring result = std::wstring(str);
	int total_index = str.size();
	for (int i = 0; i < num_parent + 1; i++)
	{
		total_index = str.substr(0, total_index).find_last_of(L"\\/");

	}
	result = str.substr(total_index, result.size());

	return result;
}

std::wstring FileSystem::GetSlashPath(const std::wstring& path)
{
	wchar_t result[1024];
	std::wstring from = L"\\";
	std::wstring to = L"/";

	int i = 0;
	for (auto ch : path)
	{
		if (ch == L'\\')
		{
			result[i++] = L'/';
		}
		else result[i++] = ch;
	}

	result[i++] = L'\0';

	return std::wstring(result);
}

std::wstring FileSystem::GetRelativeFilePath(const std::wstring& path)
{
	std::filesystem::path p = std::filesystem::absolute(path);
	std::filesystem::path r = std::filesystem::absolute(GetWorkingDirectoryA());

	if (p.root_path() != r.root_path())
		return p.generic_wstring();

	std::filesystem::path result;

	std::filesystem::path::const_iterator iter_p = p.begin();
	std::filesystem::path::const_iterator iter_r = r.begin();

	while (*iter_p == *iter_r && iter_p != p.end() && iter_r != r.end()) {
		iter_p++;
		iter_r++;
	}

	if (iter_r != r.end())
	{
		iter_r++;
		while (iter_r != r.end())
		{
			result /= "..";
			iter_r++;
		}
	}

	while (iter_p != p.end())
	{
		result /= *iter_p;
		iter_p++;
	}

	return result.generic_wstring();
}

std::vector<std::wstring> FileSystem::GetPartsOfString(std::wstring_view parted, std::wstring_view dividing)
{
	std::vector<std::wstring> names;
	std::wstring pureName = GetIntactFileNameFromPath(parted);

	size_t lastIndex = pureName.find_last_of(dividing);
	std::wstring name = pureName.substr(0, lastIndex);
	std::wstring arg = pureName.substr(lastIndex + dividing.size(), pureName.length());

	names.emplace_back(name); names.emplace_back(arg);

	return names;
}