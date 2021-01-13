#pragma once

#pragma warning(push, 0)

//=========== Extra Library ==============
#pragma comment (lib, "FreeImage.lib")
#pragma comment (lib, "libfbxsdk.lib")


#pragma warning(pop)

#define NOMINMAX

typedef unsigned int uint;

#include <string>
#include <iostream>
#include <Windows.h>
#include "Math/Math.h"
#include "Util/FileSystem.h"
#include "Util/Log/Log.h"

#define SAFE_RELEASE(p) { if(p){p->Release(); p=nullptr;}}
#define SAFE_DELETE(p) { if(p){delete p; p=nullptr;}}

// ===================================
#define CIL false  

#define Bone_Size 250   // maximum 4096 bytes 인걸 유의

#define Relative_BasisW std::wstring(CIL ? L"../../../../" : L"../../")
#define Relative_BasisA std::string(CIL ?  "../../../../" :  "../../")

#define None_StringA "N/A"
#define None_StringW L"N/A"

#define Extension_MeshA ".mesh"
#define Extension_SkMeshA ".skmesh"
#define Extension_MaterialA ".material"
#define Extension_SkeletonA ".skeleton"
#define Extension_TextureA ".texture"
#define Extension_AnimationA ".anim"

#define Extension_MeshW L".mesh"
#define Extension_SkMeshW L".skmesh"
#define Extension_MaterialW L".material"
#define Extension_SkeletonW L".skeleton"
#define Extension_TextureW L".texture"
#define Extension_MorphW L".morph"
#define Extension_AnimationW L".anim"