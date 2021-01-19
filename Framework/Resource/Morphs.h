#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include "IResource.h"

namespace Framework
{
	class Morph
	{
	public:
		enum class MorphType : unsigned int
		{
			Group = 0,
			Vertex = 1,
			Bone = 2,
			UV = 3,
			Matrial = 4,
			Flip = 5,
			Impulse = 6,
		};

		enum class MorphCategory
		{
			Reserved = 0,
			Eyebrow = 1,
			Eye = 2,
			Mouth = 3,
			Other = 4,
		};

		struct Vertex_Offset
		{
			void SaveToFile(FileStream& stream) const;
			void LoadFromFile(FileStream& stream);

			Vertex_Offset() : vertex_index(0), offset(0) {}
			Vertex_Offset(int v_index, Vector3 offset) : vertex_index(v_index), offset(offset) {}
			int vertex_index;
			Vector3 offset;
		};

		struct UV_Offset
		{
			void SaveToFile(FileStream& stream) const;
			void LoadFromFile(FileStream& stream);

			UV_Offset() : vertex_index(0), offset(0) {}
			UV_Offset(int v_index, Vector2 offset) : vertex_index(v_index), offset(offset) {}
			int vertex_index;
			Vector2 offset;
		};

		struct Group_Offset
		{
			void SaveToFile(FileStream& stream) const;
			void LoadFromFile(FileStream& stream);

			Group_Offset() : morph_index(0), weight(0) {}
			Group_Offset(int g_index, float weight) : morph_index(g_index), weight(weight) {}
			int morph_index;
			float weight;
		};

		struct Material_Offset
		{
			void SaveToFile(FileStream& stream) const;
			void LoadFromFile(FileStream& stream);
			
			Material_Offset() : material_index(-1) {}

			int material_index;
			uint8_t offset_operation;
			Color4 diffuse;
			Vector3 specular;
			float specularity;
			Vector3 ambient;
			Color4 edge_color;
			float edge_size;
			Color4 texture_rgba;
			Color4 sphere_texture_rgba;
			Color4 toon_texture_rgba;
		};

	public:
		Morph() {}
		virtual ~Morph() {}

		Morph(Morph& rhs) = delete;
		Morph(Morph&& rhs) = delete;
		Morph& operator=(Morph& rhs) = delete;
		Morph& operator=(Morph&& rhs) = delete;

		virtual bool LoadFromFile(FileStream& stream);
		virtual bool SaveToFile(FileStream& path) const;
		virtual void Clear();

	public:
		std::vector<Vertex_Offset>& Get_VertexOffsets() { return _vertex_offsets; }
		std::vector<UV_Offset>& Get_UVOffsets() { return _uv_offsets; }
		std::vector<Group_Offset>& Get_GroupOffsets() { return _group_offsets; }
		std::vector<Material_Offset>& Get_MaterialOffsets() { return _material_offsets; }

		MorphType Get_MorphType() { return _morph_type; }
		void Set_MorphType(MorphType var) { _morph_type = var; }

		MorphCategory Get_MorphCategory() { return _morph_category; }
		void Set_MorphCategory(MorphCategory var) { _morph_category = var; }

		std::wstring Get_MorphName() { return _morph_name; }
		void Set_MorphName(std::wstring_view var) { _morph_name = var; }

	protected:
		std::vector<Vertex_Offset> _vertex_offsets;
		std::vector<UV_Offset> _uv_offsets;
		std::vector<Group_Offset> _group_offsets;
		std::vector<Material_Offset> _material_offsets;

		std::wstring _morph_name;
		MorphType _morph_type;
		MorphCategory _morph_category;
	};

	class Morphs : public IResource, public std::enable_shared_from_this<Morphs>
	{
	public:
		Morphs(Context* context);
		virtual ~Morphs();

		Morphs(Morphs& rhs) = delete;
		Morphs(Morphs&& rhs) = delete;
		Morphs& operator=(Morphs& rhs) = delete;
		Morphs& operator=(Morphs&& rhs) = delete;

		virtual bool LoadFromFile(std::wstring_view path) override;
		virtual bool SaveToFile(std::wstring_view path) const override;
		virtual void Clear() override;

		Morph* AddMorph() { return _morphs.emplace_back(std::make_unique<Morph>()).get(); }
		std::vector<std::unique_ptr<class Morph>>& GetMorphs() { return _morphs; }

	protected:
		std::vector<std::unique_ptr<class Morph>> _morphs;
	};
}