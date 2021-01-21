# MMD2FBX

MikuMiku Dance make characters dance by using Inverse Kinetic (IK) which makes converting it to other format so hard.

This project converts VMD file to FBX file by recording positions of IK Bones on every tick thus no more need to calculate IK.

Also it can record the results of physics simulation that you don't have to run your physics engine. 

## Build
To Build this project you need two Library. FreeImage, Bullet Physics and FBXSDK.

I used FBXSDK to export binary data to fbx format. 

Bullet Physics is used to simulate rigidbodys of a mmd model.

Actually, FreeImage is not need for the purpose of this project, but for private purpose I included that.

You must include FBXSDK headers with Processor Definition "FBXSDK_SHARED".

## Application

"MMD2FBX" use winapi but all the functions for the purpose is implemented in "Framework" project. 

## Note

Fbx use euler angles so animation using values upper 180 degree makes fbx animation awkward in applications like 3D View.

If you convert the euler angles to quaternion, the strange animation will be fixed.


FBX SDK do not accept japanese. 

So I decided to use "TEX" folder as a texture folder of mmd.

Please alter your texture folder of a mmd file.

Also, Texture written with japanese is not accepted. 



