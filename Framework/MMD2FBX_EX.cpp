#include "Framework.h"
#include "MMD2FBX.h"


bool MMD2FBX::Export_Example(std::string_view path_refer, std::string_view path_save)
{	
	if (_manager == nullptr)
		return this->Log("_manager is not available");

	FbxImporter* importer = FbxImporter::Create(_manager, "");

	_scene_refer = FbxScene::Create(_manager, "");

	bool r = importer->Initialize(path_refer.data(), -1, _manager->GetIOSettings());
	if (!r)
		return this->Log("Importer Initialize Fail");
	
	r = importer->Import(_scene_refer);
	if (!r)
		return this->Log("Import Fail");

	importer->Destroy(); // 메모리 때문에

	FbxScene* scene = FbxScene::Create(_manager, "");
	SaveExScene(scene);

	FbxExporter* exporter = FbxExporter::Create(_manager, "");
	r = exporter->Initialize(path_save.data(), -1, _manager->GetIOSettings());
	if (!r)
		return this->Log("Exporter Initialize Fail");	
	
	r = exporter->Export(scene);
	if (!r)
		return this->Log("Import Fail");
	
	exporter->Destroy();

	return true;
}

bool MMD2FBX::SaveExScene(FbxScene* scene)
{
	FbxDocumentInfo* sceneInfo = FbxDocumentInfo::Create(_manager, "SceneInfo");
	sceneInfo->mTitle = "Example scene";
	sceneInfo->mSubject = "Illustrates the creation and animation of a deformed cylinder.";
	sceneInfo->mAuthor = "ExportScene01.exe sample program.";
	sceneInfo->mRevision = "rev. 1.0";
	sceneInfo->mKeywords = "deformed cylinder";
	sceneInfo->mComment = "no particular comments required.";
	// we need to add the sceneInfo before calling AddThumbNailToScene because
	 // that function is asking the scene for the sceneInfo.
	scene->SetSceneInfo(sceneInfo);
	
	auto root = scene->GetRootNode();

	auto node_mesh = FbxNode::Create(_manager, "Mesh0");
	root->AddChild(node_mesh);
	SaveExMesh(scene, node_mesh);

	auto node_bone_root = FbxNode::Create(scene, "Bone_Root");
	root->AddChild(node_bone_root);
	SaveExSkeleton(scene, node_bone_root);

	SaveExCluster(scene, node_mesh, node_bone_root); // skeleton 만들고 이거 안하면 바로 에러남
	SaveExBindPos(scene, node_mesh);
	SaveExRestPos(scene, node_bone_root);

	SaveExAnimation(scene, node_bone_root);

	return true;
}

bool MMD2FBX::SaveExMesh(FbxScene* scene, FbxNode* node_mesh)
{
	std::vector<FbxVector4> vertices =
	{
		FbxVector4(-0.5f,   -0.5f,   -0.5f),
		FbxVector4(+0.5f,   -0.5f,   -0.5f),
		FbxVector4(+0.5f,   -0.5f,   +0.5f),
		FbxVector4(-0.5f,   -0.5f,   +0.5f),

		FbxVector4(-0.5f,   -0.0f,   -0.5f),
		FbxVector4(+0.5f,   -0.0f,   -0.5f),
		FbxVector4(+0.5f,   -0.0f,   +0.5f),
		FbxVector4(-0.5f,   -0.0f,   +0.5f),

		FbxVector4(-0.5f,   +0.5f,   -0.5f),
		FbxVector4(+0.5f,   +0.5f,   -0.5f),
		FbxVector4(+0.5f,   +0.5f,   +0.5f),
		FbxVector4(-0.5f,   +0.5f,   +0.5f)
	};

	std::vector<int> indices =
	{
		0, 1, 2,
		0, 2, 3,
		3, 0, 4,
		3, 4, 7,
		0, 1, 5,
		0, 5, 4,
		1, 2, 6,
		1, 6, 5,
		2, 3, 7,
		2, 7, 6,
		4, 5, 6,
		4, 6, 7,
		7, 4, 8,
		7, 8, 11,
		4, 5, 9,
		4, 9, 8,
		5, 6, 10,
		5, 10, 9,
		6, 7, 11,
		6, 11, 10,
		8, 9, 10,
		8, 10, 11,
	};

	auto mesh = FbxMesh::Create(_manager, "Mesh0");
	node_mesh->SetNodeAttribute(mesh);

	mesh->InitControlPoints(vertices.size());
	mesh->mControlPoints.Resize(vertices.size());  // 비슷한 일을 함
	memcpy(mesh->mControlPoints.GetArray(), vertices.data(), sizeof(FbxVector4) * vertices.size());

	for (int n_face = 0; n_face < indices.size() / 3; n_face++)
	{
		mesh->BeginPolygon(n_face);  // polygon 마다 면을 이루는 갯수가 다를 수가 있어서 ㅈ같음
		{
			for (int k = 0; k < 3; k++)
			{
				mesh->AddPolygon(indices[n_face * 3 + k]);
			}
		}
		mesh->EndPolygon();
	}

	return true;
}

bool MMD2FBX::SaveExSkeleton(FbxScene* scene, FbxNode* node_bone_root)
{
	// Create skeleton root. 
	FbxSkeleton* boneAttribute_root = FbxSkeleton::Create(scene, "Root");
	boneAttribute_root->SetSkeletonType(FbxSkeleton::eRoot);
	node_bone_root->SetNodeAttribute(boneAttribute_root);
	node_bone_root->LclTranslation.Set(FbxVector4(0.0, -0.5, 0.0));

	// Create skeleton first limb node. 
	FbxSkeleton* boneAttribute_limb1 = FbxSkeleton::Create(scene, "Limb1");
	boneAttribute_limb1->SetSkeletonType(FbxSkeleton::eLimb);
	boneAttribute_limb1->Size.Set(1.0);
	FbxNode* node_bone_limb1 = FbxNode::Create(scene, "Limb1");
	node_bone_limb1->SetNodeAttribute(boneAttribute_limb1);
	node_bone_limb1->LclTranslation.Set(FbxVector4(0.0, 0.5, 0.0));

	// Create skeleton second limb node. 
	FbxSkeleton* boneAttribute_limb2 = FbxSkeleton::Create(scene, "Limb2");
	boneAttribute_limb2->SetSkeletonType(FbxSkeleton::eLimbNode);
	boneAttribute_limb2->Size.Set(1.0);
	FbxNode* node_bone_limb2 = FbxNode::Create(scene, "Limb2");
	node_bone_limb2->SetNodeAttribute(boneAttribute_limb2);
	node_bone_limb2->LclTranslation.Set(FbxVector4(0.0, 0.5, 0.0));

	// Build skeleton node hierarchy.
	node_bone_root->AddChild(node_bone_limb1);
	node_bone_limb1->AddChild(node_bone_limb2);

	return true;
}

bool MMD2FBX::SaveExCluster(FbxScene* scene, FbxNode* node_mesh, FbxNode* node_bone_root)
{
	int i, j;
	FbxAMatrix lXMatrix;

	FbxNode* bone_root = node_bone_root;
	FbxNode* bone_limb1 = bone_root->GetChild(0);
	FbxNode* bone_limb2 = bone_limb1->GetChild(0);
	
	// Bottom section of Box is clustered to skeleton root.
	FbxCluster* lClusterToRoot = FbxCluster::Create(scene, "");
	lClusterToRoot->SetLink(bone_root);
	lClusterToRoot->SetLinkMode(FbxCluster::eTotalOne);
	for (i = 0; i < 3; ++i)
		for (j = 0; j < 4; ++j)
			lClusterToRoot->AddControlPointIndex(4 * i + j, i == 0 ? 1.f : i == 1 ? 0.25 : 0.0f);

	// Center section of Box is clustered to skeleton limb node.
	FbxCluster* lClusterToLimbNode1 = FbxCluster::Create(scene, "");
	lClusterToLimbNode1->SetLink(bone_limb1);
	lClusterToLimbNode1->SetLinkMode(FbxCluster::eTotalOne);
	for (i = 1; i < 2; ++i)
		for (j = 0; j < 4; ++j)
			lClusterToLimbNode1->AddControlPointIndex(4 * i + j, 0.5f);


	// Top section of cylinder is clustered to skeleton limb.
	FbxCluster* lClusterToLimbNode2 = FbxCluster::Create(scene, "");
	lClusterToLimbNode2->SetLink(bone_limb2);
	lClusterToLimbNode2->SetLinkMode(FbxCluster::eTotalOne);
	for (i = 0; i < 3; ++i)
		for (j = 0; j < 4; ++j)
			lClusterToLimbNode2->AddControlPointIndex(4 * i + j, i == 2 ? 1.f : i == 1 ? 0.25 : 0.0f);

	// Now we have the Patch and the skeleton correctly positioned,
	// set the Transform and TransformLink matrix accordingly.
	FbxScene* lScene = node_mesh->GetScene();
	if (lScene) lXMatrix = node_mesh->EvaluateGlobalTransform();

	lClusterToRoot->SetTransformMatrix(lXMatrix);
	lClusterToLimbNode1->SetTransformMatrix(lXMatrix);
	lClusterToLimbNode2->SetTransformMatrix(lXMatrix);

	if (lScene) lXMatrix = bone_root->EvaluateGlobalTransform();
	lClusterToRoot->SetTransformLinkMatrix(lXMatrix);
	
	if (lScene) lXMatrix = bone_limb1->EvaluateGlobalTransform();
	lClusterToLimbNode1->SetTransformLinkMatrix(lXMatrix);
	
	if (lScene) lXMatrix = bone_limb2->EvaluateGlobalTransform();
	lClusterToLimbNode2->SetTransformLinkMatrix(lXMatrix);


	// Add the clusters to the mesh by creating a skin and adding those clusters to that skin.
	// After add that skin.
	// 다시말해서 mesh 들에 cluster 를 적용 즉 스키닝 하는거.
	FbxGeometry* lPatchAttribute = (FbxGeometry*)node_mesh->GetNodeAttribute();
	FbxSkin* lSkin = FbxSkin::Create(scene, "");
	lSkin->AddCluster(lClusterToRoot);
	lSkin->AddCluster(lClusterToLimbNode1);
	lSkin->AddCluster(lClusterToLimbNode2);
	lPatchAttribute->AddDeformer(lSkin);
	
	return true;
}

bool MMD2FBX::SaveExBindPos(FbxScene* scene, FbxNode* node_mesh)
{
	// In the bind pose, we must store all the link's global matrix at the time of the bind.
	// Plus, we must store all the parent(s) global matrix of a link, even if they are not
	// themselves deforming any model.

	// In this example, since there is only one model deformed, we don't need walk through 
	//	 the scene
	//

	if (!node_mesh || node_mesh->GetNodeAttribute() == nullptr)
		return this->Log("Mesh is not available");


	// Now list the all the link involve in the patch deformation
	std::vector<FbxNode*> node_array;
	
	int lSkinCount = 0;
	int lClusterCount = 0;
	switch (node_mesh->GetNodeAttribute()->GetAttributeType())
	{
	default:
		break;
	case FbxNodeAttribute::eMesh:
	case FbxNodeAttribute::eNurbs:
	case FbxNodeAttribute::ePatch:
		lSkinCount = ((FbxGeometry*)node_mesh->GetNodeAttribute())->GetDeformerCount(FbxDeformer::eSkin);
		for (int i = 0; i < lSkinCount; ++i)
		{
			FbxSkin* lSkin = (FbxSkin*)((FbxGeometry*)node_mesh->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
			lClusterCount += lSkin->GetClusterCount();
		}
		break;
	}
	//if we found some clusters we must add the node
	if (lClusterCount)
	{
		//Again, go through all the skins get each cluster link and add them
		for (int i = 0; i < lSkinCount; ++i)
		{
			FbxSkin* lSkin = (FbxSkin*)((FbxGeometry*)node_mesh->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
			lClusterCount = lSkin->GetClusterCount();
			for (int j = 0; j < lClusterCount; ++j)
			{
				FbxNode* lClusterNode = lSkin->GetCluster(j)->GetLink();
				AddNodeRecursively(node_array, lClusterNode);
			}

		}
		// Add the MeshNode to the pose
		node_array.push_back(node_mesh);
	}

	// Now create a bind pose with the link list
	if (node_array.size())
	{
		// A pose must be named. Arbitrarily use the name of the patch node.
		FbxPose* lPose = FbxPose::Create(scene, node_mesh->GetName());

		// default pose type is rest pose, so we need to set the type as bind pose
		lPose->SetIsBindPose(true);

		for (int i = 0; i < node_array.size(); i++)
		{
			FbxNode* lKFbxNode = node_array[i];
			FbxMatrix lBindMatrix = lKFbxNode->EvaluateGlobalTransform();

			lPose->Add(lKFbxNode, lBindMatrix);
		}

		// Add the pose to the scene
		scene->AddPose(lPose);
	}

	return true;
}

bool MMD2FBX::SaveExRestPos(FbxScene* scene, FbxNode* node_bone_root)
{
	// This example show an arbitrary rest pose assignment.
// This rest pose will set the bone rotation to the same value 
// as time 1 second in the first stack of animation, but the 
// position of the bone will be set elsewhere in the scene.
	FbxString     lNodeName;
	FbxNode* lKFbxNode;
	FbxMatrix  lTransformMatrix;
	FbxVector4 lT, lR, lS(1.0, 1.0, 1.0);

	// Create the rest pose
	FbxPose* lPose = FbxPose::Create(scene, "A Bind Pose");

	// Set the skeleton root node to the global position (10, 10, 10)
	// and global rotation of 45deg along the Z axis.
	lT.Set(10.0, 10.0, 10.0);
	lR.Set(0.0, 0.0, 45.0);

	lTransformMatrix.SetTRS(lT, lR, lS);

	// Add the skeleton root node to the pose
	lKFbxNode = node_bone_root;
	lPose->Add(lKFbxNode, lTransformMatrix, false /*it's a global matrix*/);

	// Set the lLimbNode1 node to the local position of (0, 40, 0)
	// and local rotation of -90deg along the Z axis. This show that
	// you can mix local and global coordinates in a rest pose.
	lT.Set(0.0, 40.0, 0.0);
	lR.Set(0.0, 0.0, -90.0);

	lTransformMatrix.SetTRS(lT, lR, lS);

	// Add the skeleton second node to the pose
	lKFbxNode = lKFbxNode->GetChild(0);
	lPose->Add(lKFbxNode, lTransformMatrix, true /*it's a local matrix*/);

	// Set the lLimbNode2 node to the local position of (0, 40, 0)
	// and local rotation of 45deg along the Z axis.
	lT.Set(0.0, 40.0, 0.0);
	lR.Set(0.0, 0.0, 45.0);

	lTransformMatrix.SetTRS(lT, lR, lS);

	// Add the skeleton second node to the pose
	lKFbxNode = lKFbxNode->GetChild(0);
	lNodeName = lKFbxNode->GetName();
	lPose->Add(lKFbxNode, lTransformMatrix, true /*it's a local matrix*/);

	// Now add the pose to the scene
	scene->AddPose(lPose);

	return true;
}

void MMD2FBX::SaveExAnimation(FbxScene* pScene, FbxNode* pSkeletonRoot)
{
	FbxString lAnimStackName;
	FbxTime lTime;
	int lKeyIndex = 0;

	FbxNode* lRoot = pSkeletonRoot;
	FbxNode* lLimbNode1 = pSkeletonRoot->GetChild(0);
	FbxNode* lLimbNode2 = lLimbNode1->GetChild(0);

	// First animation stack.
	lAnimStackName = "Bend on 2 sides";
	FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, lAnimStackName);

	// The animation nodes can only exist on AnimLayers therefore it is mandatory to
	// add at least one AnimLayer to the AnimStack. And for the purpose of this example,
	// one layer is all we need.
	FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
	lAnimStack->AddMember(lAnimLayer);

	// Create the AnimCurve on the Rotation.Z channel
	FbxAnimCurve* lCurve = lLimbNode2->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
	if (lCurve)
	{
		lCurve->KeyModifyBegin();
		for (int i = 0; i < 19; i++)
		{
			lTime.SetSecondDouble(i);
			lKeyIndex = lCurve->KeyAdd(lTime);
			lCurve->KeySetValue(lKeyIndex, i * 20);
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
		}
		lCurve->KeyModifyEnd();
	}

	//lCurve = lLimbNode2->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
	//if (lCurve)
	//{
	//	lCurve->KeyModifyBegin();
	//	for (int i = 0; i < 19; i++)
	//	{
	//		lTime.SetSecondDouble(i);
	//		lKeyIndex = lCurve->KeyAdd(lTime);
	//		lCurve->KeySetValue(lKeyIndex, -i * 20);
	//		lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//	}
	//	lCurve->KeyModifyEnd();
	//}

	// Same thing for the next object
	
	//lCurve = lLimbNode1->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_SCALING, true);
	//if (lCurve)
	//{
	//	lCurve->KeyModifyBegin();
	//	lTime.SetSecondDouble(0.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, 0.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//
	//	lTime.SetSecondDouble(1.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, -5.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//
	//	lTime.SetSecondDouble(2.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, 5.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//
	//	lTime.SetSecondDouble(3.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, 0.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//	lCurve->KeyModifyEnd();
	//}
	//
	//// Second animation stack.
	//lAnimStackName = "Bend and turn around";
	//lAnimStack = FbxAnimStack::Create(pScene, lAnimStackName);
	//
	//// The animation nodes can only exist on AnimLayers therefore it is mandatory to
	//// add at least one AnimLayer to the AnimStack. And for the purpose of this example,
	//// one layer is all we need.
	//lAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
	//lAnimStack->AddMember(lAnimLayer);
	//
	//// Create the AnimCurve on the Rotation.Y channel
	//lCurve = lRoot->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
	//if (lCurve)
	//{
	//	lCurve->KeyModifyBegin();
	//	lTime.SetSecondDouble(0.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, 0.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//
	//	lTime.SetSecondDouble(2.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, 10.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//	lCurve->KeyModifyEnd();
	//}
	//
	//lCurve = lLimbNode1->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
	//if (lCurve)
	//{
	//	lCurve->KeyModifyBegin();
	//	lTime.SetSecondDouble(0.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, 0.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//
	//	lTime.SetSecondDouble(1.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, 5.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//
	//	lTime.SetSecondDouble(2.0);
	//	lKeyIndex = lCurve->KeyAdd(lTime);
	//	lCurve->KeySetValue(lKeyIndex, 0.0);
	//	lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
	//	lCurve->KeyModifyEnd();
	//}
}

void MMD2FBX::AddNodeRecursively(std::vector<FbxNode*>& nodeArray, FbxNode* node)
{
	if (node)
	{
		AddNodeRecursively(nodeArray, node->GetParent());

		for (const auto& in_node : nodeArray)
		{
			if (in_node == node) // Node not in the list, add it			
				return;			
		}
		nodeArray.push_back(node);
	}
}
