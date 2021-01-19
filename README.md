# MMD2FBX

MikuMiku Dance makes characters dance by using Inverse Kinetic (IK) which makes converting it to other format so hard.

This project converts VMD file to FBX file by recording positions of IK Bones on every tick thus no more need to calculate IK.

## Build
To Build this project you need two Library. FreeImage and FBXSDK.

I used FBXSDK to export binary data to fbx format. 

Actually, FreeImage is not need for the purpose of this project, but for private purpose I included that.

You must include FBXSDK headers with Processor Definition "FBXSDK_SHARED".

## Environment

"MMD2FBX" use winapi but all the functions for the purpose is implemented in "Framework" project. 

## Note
fbx use euler angles so animation using a value upper 180 degree makes fbx animation awkward in applications like 3D View.

If you convert the euler angles to quaternion, the strange animation will be fixed.



