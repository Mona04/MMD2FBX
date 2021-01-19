#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include "Mesh.h"

namespace Framework
{
	class SkeletalMesh : public Mesh
	{
	public:
		SkeletalMesh(Context* context);
		virtual ~SkeletalMesh();

		SkeletalMesh(SkeletalMesh& rhs) = delete;
		SkeletalMesh(SkeletalMesh&& rhs) = delete;
		SkeletalMesh& operator=(SkeletalMesh& rhs) = delete;
		SkeletalMesh& operator=(SkeletalMesh&& rhs) = delete;

		virtual bool LoadFromFile(std::wstring_view path) override;
		virtual bool SaveToFile(std::wstring_view path) const override;
	};
}