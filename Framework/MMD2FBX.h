#pragma once
#include <vector>
#include <string>
#include <fbxsdk.h>

using namespace fbxsdk;

class Channel_Data
{
public:
	std::vector<std::pair<float, FbxVector4>>    _posKeys;
	std::vector<std::pair<float, FbxVector4>>    _sclKeys;
	std::vector<std::pair<float, FbxVector4>>	 _rotKeys;
};

namespace Framework
{
	class Context;
	class Actor;
	class Context;
	class Mesh;
	class Material;
	class Transform;
	class Animator;
}

class MMD2FBX
{
public:
	MMD2FBX();
	virtual ~MMD2FBX();

	void Clear();

	void Set_ms_per_tick(float var) { _ms_per_tick = var; }

private:
	bool Log(std::string_view str);
public:
	bool Export_Example(std::string_view path_refer, std::string_view path_save);

	bool Init(Framework::Actor* actor);
	bool Record_Animation_Frame(int cur_tik);
	bool Export(const std::string_view pFile);

private:
	bool SaveScene(FbxScene* scene);
	bool SaveMesh(FbxScene* scene, FbxNode* node, std::shared_ptr<Framework::Mesh> srcMesh, int index_start, int index_end);
	bool SaveMaterial(FbxScene* scene, FbxMesh* mesh, std::shared_ptr<Framework::Material> srcMaterial);
	
	FbxNode* SaveSkeleton(FbxScene* scene, FbxNode* parent, Framework::Transform* transform);
	bool SaveSkeletonRecursive(FbxScene* scene, FbxNode* parent, Framework::Transform* transform);

	bool SaveCluster(FbxScene* scene, FbxNode* node_mesh, FbxNode* node_bone_root, std::shared_ptr<Framework::Mesh> srcMesh, int index_start, int index_end);
	bool SaveBindPos(FbxScene* scene, FbxNode* node_mesh);
	bool SaveRestPos(FbxScene* scene, FbxNode* node_bone_root);
	void SaveAnimation(FbxScene* pScene, FbxNode* pSkeletonRoot);

	void GetBoneNodeArray(FbxNode* parent, std::vector<FbxNode*>& boneNode_array);

private:
	bool SaveExScene(FbxScene* scene);
	bool SaveExMesh(FbxScene* scene, FbxNode* node);
	bool SaveExSkeleton(FbxScene* scene, FbxNode* node_mesh);
	bool SaveExCluster(FbxScene* scene, FbxNode* node_mesh, FbxNode* node_bone_root);
	bool SaveExBindPos(FbxScene* scene, FbxNode* node_mesh);
	bool SaveExRestPos(FbxScene* scene, FbxNode* node_bone_root);
	void SaveExAnimation(FbxScene* pScene, FbxNode* pSkeletonRoot);

	void AddNodeRecursively(std::vector<FbxNode*>& pNodeArray, FbxNode* pNode);


private:;
    // fbx make node for each mesh. so src_bone_num may different with fbx_bone_num.

	std::string _diffuse_element_name = "Diffuse";
	std::string _ambient_element_name = "Ambient";
	std::string _emissive_element_name = "Emissive";

	FbxManager* _manager = nullptr;
	FbxScene* _scene_refer = nullptr;

	std::vector<Channel_Data> _channels_data;
	Framework::Actor* _actor;

	float _ms_per_tick = 30.f;
};