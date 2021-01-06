#include "Framework.h"
#include "MMD2FBX.h"

#include "Scene/Actor.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Animator.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Resource/Animation.h"
#include "Core/DirectX/0_IADesc/Input_Desc.h"
#include "Core/Subsystem/Resource/ResourceManager.h"

using namespace Framework;
using namespace fbxsdk;

using fbxsdk::FbxLayerElementNormal;
using fbxsdk::FbxLayerElementBinormal;
using fbxsdk::FbxLayerElementUV;
using fbxsdk::FbxAnimCurve;
using fbxsdk::FbxAnimStack;

MMD2FBX::MMD2FBX() : _actor(nullptr), _manager(nullptr)
{
}

MMD2FBX::~MMD2FBX()
{
	if(_manager)
		_manager->Destroy();  // 계층적으로 메모리 해제됨
}

void MMD2FBX::Clear()
{
	if (_manager)
	{
		_manager->Destroy();
		_manager = nullptr;
		_scene_refer = nullptr;
	}
	for (auto& channel : _channels_data)
	{
		channel._posKeys.clear();
		channel._rotKeys.clear();
		channel._sclKeys.clear();
	}
	_channels_data.clear();
	_actor = nullptr;
}

bool MMD2FBX::Log(std::string_view str)
{
	OutputDebugStringA(str.data());
	OutputDebugStringA("\n");
	return false;
}

bool MMD2FBX::Init(Actor* actor)
{
	_actor = actor;
	auto _num_src_bone = actor->GetComponent<Transform>()->Get_Transform_Array().size();
	_channels_data = std::vector<Channel_Data>(_num_src_bone);

	_manager = FbxManager::Create();
	if (_manager)
	{
		Log("Autodesk FBX SDK version" + std::string(_manager->GetVersion()) + "\n");

		FbxIOSettings* ios = FbxIOSettings::Create(_manager, IOSROOT);
		_manager->SetIOSettings(ios);
		FbxString lPath = FbxGetApplicationDirectory();
		_manager->LoadPluginsDirectory(lPath.Buffer());
	}
	else
		Log("Failed to create manager");

	return true;
}

bool MMD2FBX::Record_Animation_Frame(int cur_tik)
{
	const auto bone_array = _actor->GetComponent<Transform>()->Get_Transform_Array();
	for (int i = 0; i < bone_array.size(); i++)
	{
		const auto bone = bone_array[i];
		if (bone)
		{
			auto pos = bone->GetCalcLocalPos();   // fbx는 world 위치를 저장함
			auto rot_q = bone->GetCalcLocalRot();
			auto rot = FbxQuaternion(rot_q.x, rot_q.y, rot_q.z, rot_q.w).DecomposeSphericalXYZ();

			_channels_data[i]._posKeys.emplace_back(std::pair(cur_tik, FbxVector4(pos.x, pos.y, pos.z)));
			_channels_data[i]._sclKeys.emplace_back(std::pair(cur_tik, FbxVector4(1, 1, 1)));
			_channels_data[i]._rotKeys.emplace_back(std::pair(cur_tik, rot));
		}
	}
	return true;
}

bool MMD2FBX::Export(const std::string_view path)
{
	if (_manager == nullptr)
		return this->Log("_manager is not available");


	FbxScene* scene = FbxScene::Create(_manager, "");
	SaveScene(scene);

	FbxExporter* exporter = FbxExporter::Create(_manager, "");
	bool r = exporter->Initialize(path.data(), -1, _manager->GetIOSettings());
	if (!r)
		return this->Log("Exporter Initialize Fail");

	r = exporter->Export(scene);
	if (!r)
		return this->Log("Import Fail");

	exporter->Destroy();
	scene->Destroy();

	return true;
}

bool MMD2FBX::SaveScene(FbxScene* scene)
{
	if (!_actor)
		return Log("Initialize must be done");

	FbxDocumentInfo* sceneInfo = FbxDocumentInfo::Create(_manager, "SceneInfo");
	sceneInfo->mTitle = "MMD2FBX scene";
	sceneInfo->mSubject = "MMD Animation is translated to FBX object. Material is not saved.";
	sceneInfo->mAuthor = "ExportScene01.exe sample program.";
	sceneInfo->mRevision = "rev. 1.0";
	sceneInfo->mKeywords = "MMD";
	sceneInfo->mComment = "no particular comments required.";
	// we need to add the sceneInfo before calling AddThumbNailToScene because
	 // that function is asking the scene for the sceneInfo.
	scene->SetSceneInfo(sceneInfo);

	auto root = scene->GetRootNode();

	auto renderable = _actor->GetComponent<Renderable>();
	auto transform = _actor->GetComponent<Transform>();
	auto meshes = renderable->GetMeshes();
	auto materials = renderable->GetMaterials();


	if (meshes.size() < 1)
		return Log("No meshes");

	auto node_bone_root = SaveSkeleton(scene, root, transform);

	int index_accumulated = 0;
	int index_end = 0;
	for (auto i = 0; i < materials.size(); i++)
	{
		auto node_mesh = FbxNode::Create(_manager, ("Mesh" + std::to_string(i)).c_str());
		root->AddChild(node_mesh);

		index_end = index_accumulated + materials[i]->Get_IndexCount();
		SaveMesh(scene, node_mesh, meshes[0], index_accumulated, index_end);
		SaveCluster(scene, node_mesh, node_bone_root, meshes[0], index_accumulated, index_end);
		index_accumulated += materials[i]->Get_IndexCount();

		SaveMaterial(scene, node_mesh->GetMesh(), materials[i]);
	}

	//SaveBindPos(scene, node_mesh);
	//SaveRestPos(scene, node_bone_root);
	//
	SaveAnimation(scene, node_bone_root);

	return true;
}

bool MMD2FBX::SaveMesh(FbxScene* scene, FbxNode* node_mesh, std::shared_ptr<Mesh> src_mesh, int index_start, int index_end)
{
	const auto& src_vertices = src_mesh->GetVertices();
	const auto& src_indices = src_mesh->GetIndices();

	std::vector<FbxVector4> vertices;
	std::vector<FbxVector4> normals;
	std::vector<FbxVector4> binormals;
	std::vector<FbxVector2> uvs;
	std::vector<int> indices(index_end - index_start);
	std::unordered_map<uint, uint> src2dst_index;

	// mmd devides one mesh by index recorded by material
	for (int i = index_start; i < index_end; i++)
	{
		auto src_index = src_indices[i];
		if (src2dst_index.find(src_index) == src2dst_index.end())
		{
			auto v = src_vertices[src_index];
			auto pos = v.pos;
			auto normal = v.normal;
			auto binormal = v.binormal;
			auto uv = v.uv;

			vertices.push_back(FbxVector4(pos.x, pos.y, pos.z));
			normals.push_back(FbxVector4(normal.x, normal.y, normal.z));
			binormals.push_back(FbxVector4(binormal.x, binormal.y, binormal.z));
			uvs.push_back(FbxVector2(uv.x, uv.y));

			src2dst_index[src_index] = vertices.size() - 1;
		}
		indices.push_back(src2dst_index[src_index]);
	}
	 

	auto mesh = FbxMesh::Create(_manager, node_mesh->GetName());
	node_mesh->SetNodeAttribute(mesh);

	//============== Control Point ============================
	mesh->InitControlPoints(vertices.size());
	mesh->mControlPoints.Resize(vertices.size());  // 비슷한 일을 함
	memcpy(mesh->mControlPoints.GetArray(), vertices.data(), sizeof(FbxVector4) * vertices.size());

	//============== Uv ===================================
	FbxLayerElementUV* uv_diffuse = FbxLayerElementUV::Create(mesh, _diffuse_element_name.c_str());
	uv_diffuse->SetMappingMode(FbxLayerElement::EMappingMode::eByControlPoint);
	uv_diffuse->SetReferenceMode(FbxLayerElement::EReferenceMode::eDirect);
	{
		uv_diffuse->GetDirectArray().AddMultiple(uvs.size());
		for (int i = 0; i < uvs.size(); i++)
			uv_diffuse->GetDirectArray().SetAt(i, uvs[i]);
	}
	int n_layer = mesh->CreateLayer();
	mesh->GetLayer(n_layer)->SetUVs(uv_diffuse);

	FbxLayerElementUV* uv_emissive = FbxLayerElementUV::Create(mesh, _emissive_element_name.c_str());
	uv_emissive->SetMappingMode(FbxLayerElement::EMappingMode::eByControlPoint);
	uv_emissive->SetReferenceMode(FbxLayerElement::EReferenceMode::eDirect);
	{
		uv_emissive->GetDirectArray().AddMultiple(uvs.size());
		for (int i = 0; i < uvs.size(); i++)
			uv_emissive->GetDirectArray().SetAt(i, uvs[i]);
	}
	n_layer = mesh->CreateLayer();
	mesh->GetLayer(n_layer)->SetUVs(uv_emissive);
	
	FbxLayerElementUV* uv_ambient = FbxLayerElementUV::Create(mesh, _ambient_element_name.c_str());
	uv_ambient->SetMappingMode(FbxLayerElement::EMappingMode::eByControlPoint);
	uv_ambient->SetReferenceMode(FbxLayerElement::EReferenceMode::eDirect);
	{
		uv_ambient->GetDirectArray().AddMultiple(uvs.size());
		for (int i = 0; i < uvs.size(); i++)
			uv_ambient->GetDirectArray().SetAt(i, uvs[i]);
	}
	n_layer = mesh->CreateLayer();
	mesh->GetLayer(n_layer)->SetUVs(uv_ambient);

	//============== Normal ===================================
	FbxLayerElementNormal* normal = FbxLayerElementNormal::Create(mesh, "Normal");
	normal->SetMappingMode(FbxLayerElement::EMappingMode::eByControlPoint);
	normal->SetReferenceMode(FbxLayerElement::EReferenceMode::eDirect);
	normal->GetDirectArray().WriteLock();
	{
		normal->GetDirectArray().AddMultiple(normals.size());
		for (int i = 0; i < normals.size(); i++)		
			normal->GetDirectArray().SetAt(i, normals[i]);		
	}
	normal->GetDirectArray().WriteUnlock();
	n_layer = mesh->CreateLayer();
	mesh->GetLayer(n_layer)->SetNormals(normal);
	
	//============== Binormal ===================================
	FbxLayerElementBinormal* binormal = FbxLayerElementBinormal::Create(mesh, "Binormal");
	binormal->SetMappingMode(FbxLayerElement::EMappingMode::eByControlPoint);
	binormal->SetReferenceMode(FbxLayerElement::EReferenceMode::eDirect);
	{
		binormal->GetDirectArray().AddMultiple(binormals.size());
		for (int i = 0; i < binormals.size(); i++)		
			binormal->GetDirectArray().SetAt(i, binormals[i]);		
	}
	n_layer = mesh->CreateLayer();
	mesh->GetLayer(n_layer)->SetBinormals(binormal);

	//============== Indices ===================================
	for (int n_face = 0; n_face < indices.size() / 3; n_face++)
	{
		mesh->BeginPolygon(n_face);  // polygon 마다 면을 이루는 갯수가 다를 수가 있어서 ㅈ같음
		{
			for (int k = 0; k < 3; k++)
				mesh->AddPolygon(indices[n_face * 3 + k]);
		}
		mesh->EndPolygon();
	}

	return true;
}

bool MMD2FBX::SaveMaterial(FbxScene* scene, FbxMesh* mesh, std::shared_ptr<Framework::Material> srcMaterial)
{
	FbxNode* mesh_node = mesh->GetNode();
	if (!mesh_node) return Log("No mesh node");

	FbxSurfacePhong* material = mesh_node->GetSrcObject<FbxSurfacePhong>(0);

	if (material == NULL)
	{
		FbxString lMaterialName = "toto";
		FbxString lShadingName = "Phong";
		FbxDouble3 lBlack(0.0, 0.0, 0.0);
		FbxDouble3 lRed(1.0, 0.0, 0.0);
		FbxDouble3 lDiffuseColor(0.75, 0.75, 0.0);

		FbxLayer* lLayer = mesh->GetLayer(0);

		FbxLayerElementMaterial* lLayerElementMaterial = FbxLayerElementMaterial::Create(mesh, lMaterialName.Buffer());

		// This allows us to control where the materials are mapped.  Using eAllSame
		// means that all faces/polygons of the mesh will be assigned the same material.
		lLayerElementMaterial->SetMappingMode(FbxLayerElement::eAllSame);
		lLayerElementMaterial->SetReferenceMode(FbxLayerElement::eIndexToDirect);

		// Save the material on the layer
		lLayer->SetMaterials(lLayerElementMaterial);

		// Add an index to the lLayerElementMaterial.  Since we have only one, and are using eAllSame mapping mode,
		// we only need to add one.
		lLayerElementMaterial->GetIndexArray().Add(0);

		material = FbxSurfacePhong::Create(scene, lMaterialName.Buffer());

		// Generate primary and secondary colors.
		material->Emissive.Set(lBlack);
		material->Ambient.Set(lRed);
		material->AmbientFactor.Set(1.);
		// Add texture for diffuse channel
		material->Diffuse.Set(lDiffuseColor);
		material->DiffuseFactor.Set(1.);
		material->TransparencyFactor.Set(0.4);
		material->ShadingModel.Set(lShadingName);
		material->Shininess.Set(0.5);
		material->Specular.Set(lBlack);
		material->SpecularFactor.Set(0.3);
		mesh_node->AddMaterial(material);
	}
	

	FbxFileTexture* lTexture = FbxFileTexture::Create(scene, "Diffuse Texture");

	// Set texture properties.
	auto path = srcMaterial->Get_TexturePath(Material::Type_Texture::Diffuse);
	auto diffuse_path = FileSystem::ToString(FileSystem::GetPartPath(path, 1));
	lTexture->SetFileName(diffuse_path.c_str()); // Resource file is in current directory.
	lTexture->SetTextureUse(FbxTexture::eStandard);
	lTexture->SetMappingType(FbxTexture::eUV);
	lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	lTexture->SetSwapUV(false);
	lTexture->SetTranslation(0.0, 0.0);
	lTexture->SetScale(1.0, 1.0);
	lTexture->SetRotation(0.0, 0.0);
	lTexture->UVSet.Set(_diffuse_element_name.c_str()); // Connect texture to the proper UV

	if (material)
		material->Diffuse.ConnectSrcObject(lTexture);

	
	lTexture = FbxFileTexture::Create(scene, "Ambient Texture");

	// Set texture properties.
	lTexture->SetFileName("gradient.jpg"); // Resource file is in current directory.
	lTexture->SetTextureUse(FbxTexture::eStandard);
	lTexture->SetMappingType(FbxTexture::eUV);
	lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	lTexture->SetSwapUV(false);
	lTexture->SetTranslation(0.0, 0.0);
	lTexture->SetScale(1.0, 1.0);
	lTexture->SetRotation(0.0, 0.0);
	lTexture->UVSet.Set(_ambient_element_name.c_str()); // Connect texture to the proper UV

	if (material)
		material->Ambient.ConnectSrcObject(lTexture);

	lTexture = FbxFileTexture::Create(scene, "Emissive Texture");

	// Set texture properties.
	lTexture->SetFileName("spotty.jpg"); // Resource file is in current directory.
	lTexture->SetTextureUse(FbxTexture::eStandard);
	lTexture->SetMappingType(FbxTexture::eUV);
	lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	lTexture->SetSwapUV(false);
	lTexture->SetTranslation(0.0, 0.0);
	lTexture->SetScale(1.0, 1.0);
	lTexture->SetRotation(0.0, 0.0);
	lTexture->UVSet.Set(_emissive_element_name.c_str()); // Connect texture to the proper UV

	if (material)
		material->Emissive.ConnectSrcObject(lTexture);

	return true;
}

FbxNode* MMD2FBX::SaveSkeleton(FbxScene* scene, FbxNode* parent, Transform* transform)
{ 
	auto node_name = FbxString(FileSystem::ToString(transform->GetName()).c_str());
	FbxNode* node = FbxNode::Create(scene, node_name);
	parent->AddChild(node);

	FbxSkeleton* boneAttribute = FbxSkeleton::Create(scene, node_name);
	boneAttribute->SetSkeletonType(FbxSkeleton::eRoot);
	//boneAttribute->Size.Set(1.0);

	node->SetNodeAttribute(boneAttribute);
	node->LclTranslation.Set(FbxVector4(0.0, 0.0, 0.0));

	for (const auto child : transform->GetChilds())
		SaveSkeletonRecursive(scene, node, child);

	return node;	
}

bool MMD2FBX::SaveSkeletonRecursive(FbxScene* scene, FbxNode* parent, Transform* transform)
{
	auto node_name = FbxString(FileSystem::ToString(transform->GetName()).c_str());
	FbxNode* node = FbxNode::Create(scene, node_name);
	parent->AddChild(node);

	FbxSkeleton* boneAttribute = FbxSkeleton::Create(scene, node_name);
	boneAttribute->SetSkeletonType(FbxSkeleton::eLimb);

	auto local_pos = transform->GetLocalPosition();
	//auto local_rot = transform->GetLocalRotation();
	//auto local_scl = transform->GetLocalScale();
	node->SetNodeAttribute(boneAttribute);
	node->LclTranslation.Set(FbxVector4(local_pos.x, local_pos.y, local_pos.z));
	//node->LclScaling.Set(FbxVector4(local_scl.x, local_rot.y, local_rot.z));
	//node->LclRotation.Set(FbxQuaternion(local_rot.x, local_rot.y, local_rot.z, local_rot.w));

	for (const auto child : transform->GetChilds())
		SaveSkeletonRecursive(scene, node, child);

	return true;
}

bool MMD2FBX::SaveCluster(FbxScene* scene, FbxNode* node_mesh, FbxNode* node_bone_root, std::shared_ptr<Framework::Mesh> srcMesh, int index_start, int index_end)
{
	std::vector<FbxNode*> boneNode_array;
	GetBoneNodeArray(node_bone_root, boneNode_array);

	const auto& src_vertices = srcMesh->GetVertices();
	const auto& src_indices = srcMesh->GetIndices();

	std::vector<FbxVector4> vertices;
	std::unordered_map<uint, uint> src2dst_index;
	std::vector<std::vector<std::pair<unsigned int, float>>> boneWeights_splitted
		= std::vector<std::vector<std::pair<unsigned int, float>>>(boneNode_array.size());

	// mmd devides one mesh by index recorded by material
	for (int i = index_start; i < index_end; i++)
	{
		auto src_index = src_indices[i];
		if (src2dst_index.find(src_index) == src2dst_index.end())
		{
			auto v = src_vertices[src_index];
			auto bone_indices = v.bone_index;
			auto bone_weights = v.bone_weight;
			vertices.push_back(FbxVector4(v.pos.x, v.pos.y, v.pos.z));

			auto v_index = vertices.size() - 1;

			boneWeights_splitted[bone_indices[0]].push_back(std::make_pair(v_index, bone_weights[0]));
			boneWeights_splitted[bone_indices[1]].push_back(std::make_pair(v_index, bone_weights[1]));
			boneWeights_splitted[bone_indices[2]].push_back(std::make_pair(v_index, bone_weights[2]));
			boneWeights_splitted[bone_indices[3]].push_back(std::make_pair(v_index, bone_weights[3]));

			src2dst_index[src_index] = v_index;
		}
	}

	std::vector<FbxCluster*> clusters;
	for (int bone_index = 0; bone_index < boneNode_array.size(); bone_index++)
	{
		FbxCluster* cluster = FbxCluster::Create(scene, "");
		cluster->SetLink(boneNode_array[bone_index]);
		cluster->SetLinkMode(FbxCluster::eTotalOne);
		for (const auto& v_index_bone_weight : boneWeights_splitted[bone_index])
			cluster->AddControlPointIndex(v_index_bone_weight.first, v_index_bone_weight.second);
		clusters.push_back(cluster);
	}

	int i, j;
	FbxAMatrix scene_matrix;
	FbxAMatrix bone_matrix;
	// Now we have the Patch and the skeleton correctly positioned,
	// set the Transform and TransformLink matrix accordingly.
	FbxScene* lScene = node_mesh->GetScene();
	if (lScene) scene_matrix = node_mesh->EvaluateGlobalTransform();
	const auto boneArray = _actor->GetComponent<Transform>()->Get_Transform_Array();

	for (auto i = 0 ; i < clusters.size(); i++)
	{
		auto cluster = clusters[i];
		auto bone = boneNode_array[i];

		cluster->SetTransformMatrix(scene_matrix);
		if (lScene) bone_matrix = bone->EvaluateGlobalTransform();
		cluster->SetTransformLinkMatrix(bone_matrix);
	}

	// Add the clusters to the mesh by creating a skin and adding those clusters to that skin.
	// After add that skin.
	FbxGeometry* lPatchAttribute = (FbxGeometry*)node_mesh->GetNodeAttribute();
	FbxSkin* skin = FbxSkin::Create(scene, "");
	for (const auto& cluster : clusters)
		skin->AddCluster(cluster);
	lPatchAttribute->AddDeformer(skin);

	return true;
}

bool MMD2FBX::SaveBindPos(FbxScene* scene, FbxNode* node_mesh)
{
	return false;
}

bool MMD2FBX::SaveRestPos(FbxScene* scene, FbxNode* node_bone_root)
{
	return false;
}

void MMD2FBX::SaveAnimation(FbxScene* pScene, FbxNode* node_bone_root)
{
	std::vector<FbxNode*> boneNode_array;
	GetBoneNodeArray(node_bone_root, boneNode_array);

	FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, "MMD Animation");
	FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
	lAnimStack->AddMember(lAnimLayer);

	FbxTime lTime;
	int lKeyIndex = 0;
	std::vector<const char*> curve_channels 
		= { FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z };

	// Create the AnimCurve on the Rotation.Z channel
	for (int type = 0; type < 3; type++)
	{
		for (int bone_index = 0; bone_index < boneNode_array.size(); bone_index++)
		{
			for (int channel_index = 0; channel_index < curve_channels.size(); channel_index++)
			{
				const auto& channel_data = _channels_data[bone_index];
				FbxAnimCurve* lCurve;
				switch (type)
				{
				case 0: 
					lCurve = boneNode_array[bone_index]->LclTranslation.GetCurve(lAnimLayer, curve_channels[channel_index], true); 
					lCurve->KeyModifyBegin();
					for (const auto& frame : channel_data._posKeys)
					{
						lTime.SetMilliSeconds(frame.first * _ms_per_tick);
						lKeyIndex = lCurve->KeyAdd(lTime);
						lCurve->KeySetValue(lKeyIndex, frame.second.mData[channel_index]);
						lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
					}
					lCurve->KeyModifyEnd();
					break;
				case 1: 
					lCurve = boneNode_array[bone_index]->LclScaling.GetCurve(lAnimLayer, curve_channels[channel_index], true);
					lCurve->KeyModifyBegin();
					for (const auto& frame : channel_data._sclKeys)
					{
						lTime.SetMilliSeconds(frame.first * _ms_per_tick);
						lKeyIndex = lCurve->KeyAdd(lTime);
						lCurve->KeySetValue(lKeyIndex, frame.second.mData[channel_index]);
						lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
					}
					lCurve->KeyModifyEnd();
					break;					
				case 2:
					lCurve = boneNode_array[bone_index]->LclRotation.GetCurve(lAnimLayer, curve_channels[channel_index], true);
					lCurve->KeyModifyBegin();
					for (const auto& frame : channel_data._rotKeys)
					{
						lTime.SetMilliSeconds(frame.first * _ms_per_tick);
						lKeyIndex = lCurve->KeyAdd(lTime);
						lCurve->KeySetValue(lKeyIndex, frame.second.mData[channel_index]);
						lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
					}
					lCurve->KeyModifyEnd();
					break;
				}			
			}
		}
	}
}

void MMD2FBX::GetBoneNodeArray(FbxNode* parent, std::vector<FbxNode*>& boneNode_array)
{
	const auto& boneArray = _actor->GetComponent<Transform>()->Get_Transform_Array();
	if (boneNode_array.size() != boneArray.size()) // actually it should be done outside recursive function. but i'm lazy
		boneNode_array.resize(boneArray.size());

	for(int i = 0; i < boneNode_array.size(); i++)
	{
		auto boneArrayName = FbxString(FileSystem::ToString(boneArray[i]->GetName()).c_str());
		if (boneArrayName == parent->GetName())
		{
			boneNode_array[i] = parent;
			break;
		}
		if (i == boneNode_array.size() - 1)
			Log("Making BoneNode Array is failded");
	}
	
	for (int i = 0; i < parent->GetChildCount(); i++)
		GetBoneNodeArray(parent->GetChild(i), boneNode_array);

}