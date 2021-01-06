#pragma once
#include "Math/Math.h"

#define Bone_Size 250

namespace Framework
{

	struct Vertex_PositionColor
	{
	public:
		Vertex_PositionColor() = default;
		Vertex_PositionColor(float x, float y, float z) : pos(x, y, z), color(1) {}
		Vertex_PositionColor(const Vector3& pos, const Color4& color) : pos(pos), color(color) {}
		virtual ~Vertex_PositionColor() = default;

	public:
		Vector3 pos;
		Color4 color;

		static const unsigned int count = 2;
	};

	struct Vertex_PositionUv
	{
	public:
		Vertex_PositionUv() = default;
		Vertex_PositionUv(const Vector3& pos, const Vector2& uv) : pos(pos), uv(uv) {}
		~Vertex_PositionUv() = default;

	public:
		Vector3 pos;
		Vector2 uv;

		static const unsigned int count = 2;
	};

	struct Vertex_PositionUvNormal
	{
	public:
		Vertex_PositionUvNormal() = default;
		Vertex_PositionUvNormal(const Vector3& pos, const Vector2& uv) : pos(pos), uv(uv), normal(0, 0, 1) {}
		Vertex_PositionUvNormal(const Vector3& pos, const Vector2& uv, const Vector3& normal) 
			: pos(pos), uv(uv), normal(normal) {}
		~Vertex_PositionUvNormal() = default;

	public:
		Vector3 pos;
		Vector2 uv;
		Vector3 normal;

		static const unsigned int count = 3;
	};

	struct Vertex_MASTER
	{
	public:
		Vertex_MASTER() : pos(pos), uv(uv), normal(0, 0, 1), edge(1), color(1)
		{
			for (unsigned int& i : bone_index)
				i = 0;
			for (float& w : bone_weight)
				w = 0;
		}
		Vertex_MASTER(const Vector3& pos, const Vector2& uv) : pos(pos), uv(uv), normal(0, 0, 1), edge(1), color(1)
		{
			for(unsigned int& i : bone_index)
				i = 0;
			for (float& w : bone_weight)
				w = 0;
		}
		Vertex_MASTER(const Vector3& pos, const Vector2& uv, const Vector3& normal)
			: pos(pos), uv(uv), normal(normal), edge(1), color(1)
		{
			for (unsigned int& i : bone_index)
				i = 0;
			for (float& w : bone_weight)
				w = 0;
		}
		Vertex_MASTER(const Vector3& pos, const Color4& color)
			: pos(pos), uv(uv), normal(0, 0, 1), edge(1), color(color)
		{
			for (unsigned int& i : bone_index)
				i = 0;
			for (float& w : bone_weight)
				w = 0;
		}

		~Vertex_MASTER() = default;

	public:
		Vector3 pos;
		Vector2 uv;
		Color4 color;
		Vector3 normal;
		Vector3 tangent;
		Vector3 binormal;

		float bone_weight[4];
		unsigned int bone_index[4];
		float edge;
		Vector4 uva[4];

		static const unsigned int count = 9;
	};

	struct GlobalData
	{
		Matrix world;
		Matrix wvp_cur;
		Matrix wvp_before;

		Matrix view;
		Matrix proj;
		Matrix viewProj;

		Vector3 camera_pos;
		float camera_near;
		float camera_far;

		Vector3 lightColor;
		Vector3 lightDirection;
		float padding[1];
	};

	struct MaterialData
	{
		Color4 diffuse;
		Vector3 specular;
		float specularlity;
		Vector3 ambient;
		Color4 edge_color;
		float edge_size;

		float draw_mode;
		float sphere_op_mode;
		float toon_mode;
		float padding;
	};

	struct BoneData
	{
		Matrix bone_worlds[Bone_Size];
		Matrix bone_wvp_cur[Bone_Size];
		Matrix bone_wvp_prev[Bone_Size]; 
	};

	struct FrameData
	{
		Matrix wvp;
		Color4 color;
		Color4 hashcode;
		int isSelected;
		float padding[3];
	};

	struct PostprocessData
	{
		Matrix world;
	};
}