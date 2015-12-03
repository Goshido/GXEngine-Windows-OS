//version 1.2

#ifndef GX_PSK_LOADER
#define GX_PSK_LOADER


#include "UnrealStructs.h"


union GXLightJoint
{
	struct
	{
		GXQuat	Orientation;
		GXVec3	Position;
	};
	GXFloat v[ 7 ];
};

union GXLightBone
{
	struct
	{
		GXChar			Name[ 64 ];
		GXUInt			ParentIndex;	// 0/NULL в случае корневой кости
		GXLightJoint	BonePos;		// Ссылка на положение в 3D пространстве
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + sizeof ( GXUInt ) + sizeof ( GXLightJoint ) ];
};

union GXVBOSkinStruct
{
	struct
	{
		GXVec4 index;
		GXVec4 weight;
	};
	GXUChar v[ 2 * sizeof ( GXVec4 ) ];
};

union GXSkeletalMeshInfo
{
	struct
	{
		GXUShort			totalVBOxyz;
		GXFloat*			VBOxyz;

		GXUShort			totalVBOskin;
		GXVBOSkinStruct*	VBOskin;

		GXUShort			totalVBOuv;
		GXFloat*			VBOuv;

		GXUShort			totalelements;
		GXUShort*			ElementArray;

		GXUShort			totalBones;
		GXLightBone*		Bones;
	};
	GXUChar v[ 5 * ( sizeof ( GXUShort ) + sizeof ( GXVoid* ) ) ];
};


GXVoid GXCALL GXLoadPSK ( const GXWChar* file_name, GXSkeletalMeshInfo* skminfo );


#endif	//GX_PSK_LOADER