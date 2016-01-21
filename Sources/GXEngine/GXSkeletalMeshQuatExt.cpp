//version 1.1

#include <GXEngine/GXSkeletalMeshQuatExt.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXStrings.h>
#include <new>


class GXBoneKeeperNodeExt : public GXAVLTreeNode
{
	public:
		const GXUTF8*	boneName;
		GXUShort		boneIndex;

	public:
		GXBoneKeeperNodeExt ( const GXChar* boneName, GXUShort boneIndex );
	
		virtual const GXVoid* GetKey ();

		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
};

GXBoneKeeperNodeExt::GXBoneKeeperNodeExt ( const GXUTF8* boneName, GXUShort boneIndex )
{
	this->boneName = boneName;
	this->boneIndex = boneIndex;
}

const GXVoid* GXBoneKeeperNodeExt::GetKey ()
{
	return boneName;
}

GXInt GXCALL GXBoneKeeperNodeExt::Compare ( const GXVoid* a, const GXVoid* b )
{
	return GXUTF8cmp ( (const GXUTF8*)a, (const GXUTF8*)b );
}

//--------------------------------------------------------------------------------------------------

class GXBoneKeeperExt : public GXAVLTree
{
	private:
		GXBoneKeeperNodeExt*		cacheFriendlyNodes;

	public:
		GXBoneKeeperExt ( const GXSkeletalMeshInfoExt &skmInfo );
		virtual ~GXBoneKeeperExt ();

		GXUShort FindBoneIndex ( const GXChar* boneName );
};

GXBoneKeeperExt::GXBoneKeeperExt ( const GXSkeletalMeshInfoExt &skmInfo ) :
GXAVLTree ( &GXBoneKeeperNodeExt::Compare, GX_FALSE )
{
	cacheFriendlyNodes = (GXBoneKeeperNodeExt*)malloc ( sizeof ( GXBoneKeeperNodeExt ) * skmInfo.numBones );
	for ( GXUShort i = 0; i < skmInfo.numBones; i++ )
	{
		new ( cacheFriendlyNodes + i ) GXBoneKeeperNodeExt ( skmInfo.boneNames + i * GX_BONE_NAME_SIZE, i );
		Add ( cacheFriendlyNodes + i );
	}
}

GXBoneKeeperExt::~GXBoneKeeperExt ()
{
	free ( cacheFriendlyNodes );
}

GXUShort GXBoneKeeperExt::FindBoneIndex ( const GXUTF8* boneName )
{
	const GXBoneKeeperNodeExt* node = (const GXBoneKeeperNodeExt*)FindByKey ( boneName );

	if ( !node )
		return 0xFFFF;

	return node->boneIndex;
}

//-------------------------------------------------------------------------------------------------

GXSkeletalMeshQuatExt::GXSkeletalMeshQuatExt ()
{
	keeper = 0;
	animSolver = 0;
	memset ( &skmInfo, 0, sizeof ( GXSkeletalMeshInfoExt ) );
}

GXSkeletalMeshQuatExt::~GXSkeletalMeshQuatExt ()
{
	GXSafeDelete ( keeper );
}

GXVoid GXSkeletalMeshQuatExt::GetBoneGlobalLocation ( GXVec3 &out, const GXUTF8* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	if ( i == 0xFFFF ) return;

	GXVec4 ans;
	GXVec4 buf ( globalPose[ i ].location, 1.0f );

	GXMulVec4Mat4 ( ans, buf, mod_mat );

	out = GXCreateVec3 ( ans.x, ans.y, ans.z );
}

GXVoid GXSkeletalMeshQuatExt::GetBoneGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXUTF8* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	if ( i == 0xFFFF ) return;

	GXVec4 ans;
	GXVec4 buf ( globalPose[ i ].location, 1.0f );
	GXMulVec4Mat4 ( ans, buf, mod_mat );
	loc = GXCreateVec3 ( ans.x, ans.y, ans.z );

	GXQuat bufRot = GXCreateQuat ( mod_mat );
	GXMulQuatQuat ( rot, globalPose[ i ].rotation, bufRot );
}

GXVoid GXSkeletalMeshQuatExt::GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXUTF8* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	if ( i == 0xFFFF ) return;

	GXVec3 ofst = GXCreateVec3 ( offset.y, offset.x, offset.z );

	GXVec3 buf;
	GXVec3 buf01;

	GXQuatTransform ( buf01, globalPose[ i ].rotation, ofst );

	GXSumVec3Vec3 ( buf, globalPose[ i ].location, buf01 );

	GXVec4 ans;
	GXVec4 buf02 ( buf, 1.0f );
	GXMulVec4Mat4 ( ans, buf02, mod_mat );

	out = GXCreateVec3 ( ans.x, ans.y, ans.z );
}

GXVoid GXSkeletalMeshQuatExt::GetAttacmentGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXVec3 &offset, const GXUTF8* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	if ( i == 0xFFFF ) return;

	GXVec3 ofst = GXCreateVec3 ( offset.y, offset.x, offset.z );

	GXVec3 buf;
	GXVec3 buf01;

	GXQuatTransform ( buf01, globalPose[ i ].rotation, ofst );

	GXSumVec3Vec3 ( buf, globalPose[ i ].location, buf01 );

	GXVec4 ans;
	GXVec4 buf02 ( buf, 1.0f );
	GXMulVec4Mat4 ( ans, buf02, mod_mat );

	loc = GXCreateVec3 ( ans.x, ans.y, ans.z );

	GXQuat bufRot = GXCreateQuat ( mod_mat );
	GXMulQuatQuat ( rot, globalPose[ i ].rotation, bufRot );
}

GXVoid GXSkeletalMeshQuatExt::SetAnimSolver ( GXAnimSolver* animSolver )
{
	this->animSolver = animSolver;
}

GXVoid GXSkeletalMeshQuatExt::Update ( GXFloat deltaTime )
{
	if ( animSolver )
		animSolver->Update ( deltaTime );
}

GXVoid GXSkeletalMeshQuatExt::InitReferencePose ()
{
	keeper = new GXBoneKeeperExt ( skmInfo );
}

GXVoid GXSkeletalMeshQuatExt::InitPose ()
{
	memcpy ( localPose, skmInfo.refPose, sizeof ( GXQuatLocJoint ) * skmInfo.numBones );
	
	if ( animSolver )
	{
		for ( GXUInt i = 0; i < skmInfo.numBones; i++ )
		{
			const GXQuat* rot = 0;
			const GXVec3* loc = 0;

			animSolver->GetBone ( skmInfo.boneNames + i * GX_BONE_NAME_SIZE, &rot, &loc );

			if ( rot )
			{
				localPose[ i ].rotation = *rot;
				localPose[ i ].location = *loc;
			}
		}
	}

	//Расчёт итоговой трансформации корневой кости. Корневая кость - особый случай (у неё нет родителя).

	globalPose[ 0 ].rotation = localPose[ 0 ].rotation;
	globalPose[ 0 ].location = localPose[ 0 ].location;

	GXMulQuatQuat ( vertexTransform[ 0 ].rotation, skmInfo.bindTransform[ 0 ].rotation, globalPose[ 0 ].rotation );
	GXVec3 vTemp2;
	GXQuatTransform ( vTemp2, vertexTransform[ 0 ].rotation, skmInfo.bindTransform[ 0 ].location );
	GXSumVec3Vec3 ( vertexTransform[ 0 ].location, vTemp2, globalPose[ 0 ].location );

	//Расчёт всех оставшихся костей.

	for ( GXUInt i = 1; i < skmInfo.numBones; i++ )
	{
		GXMulQuatQuat ( globalPose[ i ].rotation, localPose[ i ].rotation, globalPose[ skmInfo.parentIndex[ i ] ].rotation );
		GXVec3 vTemp;
		GXQuatTransform ( vTemp, globalPose[ skmInfo.parentIndex[ i ] ].rotation, localPose[ i ].location );
		GXSumVec3Vec3 ( globalPose[ i ].location, vTemp, globalPose[ skmInfo.parentIndex[ i ] ].location );

		GXMulQuatQuat ( vertexTransform[ i ].rotation, skmInfo.bindTransform[ i ].rotation, globalPose[ i ].rotation );
		GXVec3 vTemp2;
		GXQuatTransform ( vTemp2, vertexTransform[ i ].rotation, skmInfo.bindTransform[ i ].location );
		GXSumVec3Vec3 ( vertexTransform[ i ].location, vTemp2, globalPose[ i ].location );
	}
}
