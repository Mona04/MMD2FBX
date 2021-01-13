#pragma once
#include <string>
#include <memory>
#include "Framework/Core/Subsystem/Context.h"
#include "Framework/Util/FileStream.h"

namespace Framework
{
	class IResource
	{
	public:
		enum class TypeCode : unsigned int { None, Texture, Script, Mesh, SkeletalMesh, Skeleton, Animation, Material, Morph };
		template<typename T> static constexpr TypeCode DeduceType();

	public:
		IResource(Context* context) : _context(context), _typecode(TypeCode::None) {};
		virtual ~IResource() = default;
		virtual void Clear() = 0;

		void SetPath(std::wstring_view path) { _path = path; }
		std::wstring GetPath() { return _path; }

	protected:
		virtual bool LoadFromFile(std::wstring_view path) = 0;
		virtual bool SaveToFile(std::wstring_view path) = 0;

	protected:
		Context* _context;

		std::wstring _path;
		TypeCode _typecode;
	};

	template<typename T>
	inline constexpr IResource::TypeCode IResource::DeduceType() { return TypeCode::None; }

#define RegisterResourceType(T, Code) template<> inline constexpr IResource::TypeCode IResource::DeduceType<T>() { return Code; }
	RegisterResourceType(class Texture, TypeCode::Texture)
		RegisterResourceType(class Script, TypeCode::Script)
		RegisterResourceType(class Mesh, TypeCode::Mesh)
		RegisterResourceType(class SkeletalMesh, TypeCode::SkeletalMesh)
		RegisterResourceType(class Skeleton, TypeCode::Skeleton)
		RegisterResourceType(class Animation, TypeCode::Animation)
		RegisterResourceType(class Material, TypeCode::Material)
		RegisterResourceType(class Morph, TypeCode::Morph)
}