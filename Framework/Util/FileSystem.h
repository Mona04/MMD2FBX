#pragma once
#include <string>
#include <vector>
#include <functional>

namespace Framework
{
	class FileSystem final
	{
	public:
		static void OpenFilePathDialogA(std::function<void(std::string)>&& function, const char* filter, const char* directory, HWND hwnd);
		static void OpenFileDialogA(std::function<void(std::string)>&& function, const char* filter, const char* directory, HWND hwnd);
		static void CreateFolder(const std::string& path);
		static bool IsExistFile(std::string_view str);
		static const std::string GetWorkingDirectoryA();

		static void OpenFilePathDialogW(std::function<void(std::wstring)>&& function, const WCHAR* filter, const WCHAR* directory, HWND hwnd);
		static void OpenFileDialogW(std::function<void(std::wstring)>&& function, const WCHAR* filter, const WCHAR* directory, HWND hwnd);
		static void CreateFolder(const std::wstring& path);
		static bool IsExistFile(std::wstring_view str);
		static const std::wstring GetWorkingDirectoryW();
		static std::vector<std::wstring> GetFilesInDirectory(const std::wstring& path);

	public:
		static std::wstring ToWString(std::string_view str, bool isJapanese = true);
		static std::string ToString(std::wstring_view str, bool isJapanese = true);

	public:
		static std::string ToAbsolute(std::string_view str);
		static std::string ToSmaller(std::string_view str);
		static std::string Find_Replace_All(std::string_view str, std::string_view oldStr, std::string_view newStr);

		static std::string GetFileDirectoryFromPath(std::string_view path);
		static std::string GetFileExtensionFromPath(std::string_view path);
		static std::string GetFileNameFromPath(std::string_view str);
		static std::string GetIntactFileNameFromPath(std::string_view str);
		static std::string GetSlashPath(const std::string& path);
		static std::string GetRelativeFilePath(const std::string& path);
		static std::vector<std::string> GetPartsOfString(std::string_view parted, std::string_view dividing);


	public:
		static std::wstring ToAbsolute(std::wstring_view str);
		static std::wstring ToSmaller(std::wstring_view str);
		static std::wstring Find_Replace_All(std::wstring_view str, std::wstring_view oldStr, std::wstring_view newStr);

		static std::wstring GetFileDirectoryFromPath(std::wstring_view path);
		static std::wstring GetFileExtensionFromPath(std::wstring_view path);
		static std::wstring GetFileNameFromPath(std::wstring_view str);
		static std::wstring GetIntactFileNameFromPath(std::wstring_view str);
		static std::wstring GetPartPath(std::wstring_view parted, int num_parent);
		static std::wstring GetSlashPath(const std::wstring& path);
		static std::wstring GetRelativeFilePath(const std::wstring& path);
		static std::vector<std::wstring> GetPartsOfString(std::wstring_view parted, std::wstring_view dividing);
	};
}