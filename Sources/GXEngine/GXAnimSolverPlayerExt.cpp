//version 1.2

#include <GXEngine/GXAnimSolverPlayerExt.h>
#include <GXEngine/GXAVLTree.h>
#include <new>


class GXBoneFinderNodeExt : public GXAVLTreeNode
{
	public:
		const GXUTF8*	boneName;
		GXUShort		boneIndex;

	public:
		GXBoneFinderNodeExt ( const GXUTF8* boneName, GXUShort boneIndex );
	
		virtual const GXVoid* GetKey ();

		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
};

GXBoneFinderNodeExt::GXBoneFinderNodeExt ( const GXUTF8* boneName, GXUShort boneIndex )
{
	this->boneName = boneName;
	this->boneIndex = boneIndex;
}

const GXVoid* GXBoneFinderNodeExt::GetKey ()
{
	return boneName;
}

GXInt GXCALL GXBoneFinderNodeExt::Compare ( const GXVoid* a, const GXVoid* b )
{
	return strcmp ( (const GXUTF8*)a, (const GXUTF8*)b );
}

//--------------------------------------------------------------------------------------------------

class GXBoneFinderExt : public GXAVLTree
{
	private:
		GXBoneFinderNodeExt*	cacheFriendlyNodes;

	public:
		GXBoneFinderExt ( const GXAnimationInfoExt &animInfo );
		virtual ~GXBoneFinderExt ();

		GXUShort FindBoneIndex ( const GXUTF8* boneName );
};

GXBoneFinderExt::GXBoneFinderExt ( const GXAnimationInfoExt &animInfo ) :
GXAVLTree ( &GXBoneFinderNodeExt::Compare, GX_FALSE )
{
	cacheFriendlyNodes = (GXBoneFinderNodeExt*)malloc ( sizeof ( GXBoneFinderNodeExt ) * animInfo.numBones );
	for ( GXUShort i = 0; i < animInfo.numBones; i++ )
	{
		new ( cacheFriendlyNodes + i ) GXBoneFinderNodeExt ( animInfo.boneNames + i * GX_BONE_NAME_SIZE, i );
		Add ( cacheFriendlyNodes + i );
	}
}

GXBoneFinderExt::~GXBoneFinderExt ()
{
	free ( cacheFriendlyNodes );
}

GXUShort GXBoneFinderExt::FindBoneIndex ( const GXUTF8* boneName )
{
	const GXBoneFinderNodeExt* node = (const GXBoneFinderNodeExt*)FindByKey ( boneName );

	if ( !node )
		return 0xFFFF;

	return node->boneIndex;
}

//--------------------------------------------------------------------------------------------------

GXAnimSolverPlayerExt::GXAnimSolverPlayerExt ( GXUShort solverID ):
GXAnimSolver ( solverID )
{
	animPos = 0.0f;
	finder = 0;
}

GXAnimSolverPlayerExt::~GXAnimSolverPlayerExt ()
{
	GXSafeDelete ( finder );
}

GXVoid GXAnimSolverPlayerExt::GetBone ( const GXUTF8* boneName, const GXQuat** rot, const GXVec3** loc )
{
	if ( !finder || !animData )
	{
		*rot = 0;
		*loc = 0;
		return;
	}

	GXUShort boneIndex = finder->FindBoneIndex ( boneName );

	if ( boneIndex == 0xFFFF )
	{
		*rot = 0;
		*loc = 0;
		return;
	}

	GXUInt frame = (GXUInt)( animPos * animData->numFrames );
	if ( frame >= animData->numFrames )
		frame = animData->numFrames - 1;

	const GXQuatLocJoint* joint = animData->keys + frame * animData->numBones + boneIndex;
	
	*rot = &joint->rotation;
	*loc = &joint->location;
}

GXVoid GXAnimSolverPlayerExt::Update ( GXFloat delta )
{
	animPos += delta * multiplier * delta2PartFartor;

	if ( animPos > 1.0f )
	{
		while ( animPos > 1.0f )
			animPos -= 1.0f;
	}
	else if ( animPos < 0.0f )
	{
		while ( animPos < 0.0f )
			animPos += 1.0f;
	}
}

GXVoid GXAnimSolverPlayerExt::SetAnimationSequence ( const GXAnimationInfoExt* animData )
{
	GXSafeDelete ( finder );
	finder = new GXBoneFinderExt ( *animData );

	this->animData = animData; 
	GXFloat totalTime = (GXFloat)animData->numFrames / animData->fps;
	delta2PartFartor = 1.0f / totalTime;
}

GXVoid GXAnimSolverPlayerExt::SetAnimationMultiplier ( GXFloat multiplier )
{
	this->multiplier = multiplier;
}