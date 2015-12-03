//version 1.2

#ifndef GX_PSA_LOADER
#define GX_PSA_LOADER


#include <GXCommon/GXTypes.h>
#include "UnrealStructs.h"


struct GXBoneNames
{
	GXChar Name[ 64 ];
};

union GXAnimSeq
{
	struct
	{
		GXChar		Name[ 64 ];
		GXFloat		FPS;
		GXInt		FirstRawFrame;
		GXInt		NumRawFrames;
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + sizeof ( GXFloat ) + 2 * sizeof ( GXInt ) ];
};

union GXSkeletalMeshAnimationInfo
{
	struct
	{
		GXUInt			totalrawkeys;
		VQuatAnimKey*	RawKeys;

		GXUInt			totalbones;
		GXBoneNames*	Bones;

		GXUInt			totalanimations;
		GXAnimSeq*		Animations;
	};
	GXUChar v[ 3 * ( sizeof ( GXUInt ) + sizeof ( GXVoid* ) ) ];
};

GXVoid GXCALL GXLoadPSA ( const GXWChar* file_name, GXSkeletalMeshAnimationInfo* skmaniminfo );


#endif	//GX_PSA_LOADER