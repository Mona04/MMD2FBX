#pragma once
#include <fstream>

namespace Framework
{
	enum class StreamMode : uint
	{
		Write, Read, AsciiWrite, AscilRead
	};

	class FileStream final
	{
	public:
		FileStream() = default;
		~FileStream() = default;

		bool Open(const std::string& path, const StreamMode& mode);
		bool Open(const std::wstring& path, const StreamMode& mode);
		void Close();

		//=========================================================
		//Write
		template <typename T, typename = typename std::enable_if<
			std::is_same<T, bool>::value ||
			std::is_same<T, char>::value ||
			std::is_same<T, byte>::value ||
			std::is_same<T, std::byte>::value ||
			std::is_same<T, int>::value ||
			std::is_same<T, long>::value ||
			std::is_same<T, unsigned int>::value ||
			std::is_same<T, unsigned long>::value ||
			std::is_same<T, long long>::value ||
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, Vector2>::value ||
			std::is_same<T, Vector3>::value ||
			std::is_same<T, Vector4>::value ||
			std::is_same<T, Quaternion>::value ||
			std::is_same<T, Color4>::value ||
			std::is_same<T, Matrix>::value ||
			std::is_same<T, uint8_t>::value ||
			std::is_same<T, UInt8Vector2>::value
		>::type> void Write(const T& value)
		{
			out.write(reinterpret_cast<const char*>(&value), sizeof(T));
		}

		void Write(const unsigned char& value)
		{
			char v = static_cast<const char>(value);
			out.write(reinterpret_cast<const char*>(&v), sizeof(unsigned char));
		}

		void Write(const uint& value) { out.write(reinterpret_cast<const char*>(&value), sizeof(uint)); }
		void Write(const std::string& value);
		void Write(const std::wstring& value);
		void Write(const std::vector<std::byte>& value);

		//=========================================================

		//=========================================================
		//Read
		template <typename T, typename = typename std::enable_if<
			std::is_same<T, bool>::value ||
			std::is_same<T, char>::value ||
			std::is_same<T, byte>::value ||
			std::is_same<T, std::byte>::value ||
			std::is_same<T, int>::value ||
			std::is_same<T, uint>::value ||
			std::is_same<T, long>::value ||
			std::is_same<T, unsigned long>::value ||
			std::is_same<T, long long>::value ||
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, Vector2>::value ||
			std::is_same<T, Vector3>::value ||
			std::is_same<T, Vector4>::value ||
			std::is_same<T, Quaternion>::value ||
			std::is_same<T, Color4>::value ||
			std::is_same<T, Matrix>::value ||
			std::is_same<T, uint8_t>::value ||
			std::is_same<T, UInt8Vector2>::value
		>::type> void Read(T& value)
		{
			in.read(reinterpret_cast<char*>(&value), sizeof(T));
		}

		void Read(std::string& value);
		void Read(std::wstring& value);
		void Read(std::vector<std::byte>& value);

		const uint ReadUInt();
		//=========================================================

		std::ifstream& GetReadStream() { return in; }
		std::ofstream& GetWriteStream() { return out; }

	private:
		StreamMode mode = StreamMode::Read;
		std::ifstream in;
		std::ofstream out;
	};
}