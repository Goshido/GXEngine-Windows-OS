//version 1.3

#include <GXEngine/GXAnimSolverPlayer.h>
#include <GXCommon/GXAVLTree.h>
#include <new>


class GXBoneFinderNode : public GXAVLTreeNode
{
	public:
		const GXUTF8*	boneName;
		GXUShort		boneIndex;

	public:
		GXBoneFinderNode ( const GXUTF8* boneName, GXUShort boneIndex );
	
		virtual const GXVoid* GetKey ();

		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
};

GXBoneFinderNode::GXBoneFinderNode ( const GXUTF8* boneName, GXUShort boneIndex )
{
	this->boneName = boneName;
	this->boneIndex = boneIndex;
}

const GXVoid* GXBoneFinderNode::GetKey ()
{
	return boneName;
}

GXInt GXCALL GXBoneFinderNode::Compare ( const GXVoid* a, const GXVoid* b )
{
	return strcmp ( (const GXUTF8*)a, (const GXUTF8*)b );
}

//--------------------------------------------------------------------------------------------------

class GXBoneFinder : public GXAVLTree
{
	private:
		GXBoneFinderNode*		cacheFriendlyNodes;

	public:
		GXBoneFinder ( const GXSkeletalMeshAnimationInfo &animInfo );
		virtual ~GXBoneFinder ();

		GXUShort FindBoneIndex ( const GXUTF8* boneName );
};

GXBoneFinder::GXBoneFinder ( const GXSkeletalMeshAnimationInfo &animInfo ) :
GXAVLTree ( &GXBoneFinderNode::Compare, GX_FALSE )
{
	cacheFriendlyNodes = (GXBoneFinderNode*)malloc ( sizeof ( GXBoneFinderNode ) * animInfo.totalbones );
	for ( GXUShort i = 0; i < animInfo.totalbones; i++ )
	{
		new ( cacheFriendlyNodes + i ) GXBoneFinderNode ( animInfo.Bones[ i ].Name, i );
		Add ( cacheFriendlyNodes + i );
	}
}

GXBoneFinder::~GXBoneFinder ()
{
	free ( cacheFriendlyNodes );
}

GXUShort GXBoneFinder::FindBoneIndex ( const GXUTF8* boneName )
{
	const GXBoneFinderNode* node = (const GXBoneFinderNode*)FindByKey ( boneName );

	if ( !node )
		return 0xFFFF;

	return node->boneIndex;
}

//--------------------------------------------------------------------------------------------------

GXAnimSolverPlayer::GXAnimSolverPlayer ( GXUShort solverID ):
GXAnimSolver ( solverID )
{
	animPos = 0.0f;
	finder = 0;
}

GXAnimSolverPlayer::~GXAnimSolverPlayer ()
{
	GXSafeDelete ( finder );
}

GXVoid GXAnimSolverPlayer::GetBone ( const GXUTF8* boneName, const GXQuat** rot, const GXVec3** loc )
{
	if ( !finder )
	{
		*rot = 0;
		*loc = 0;
		return;
	}

	#define GET_RAW_FRAME(anim,frame) \
	( ( animData->Animations[ anim ].FirstRawFrame + frame ) * animData->totalbones )

	GXUShort i = finder->FindBoneIndex ( boneName );
	
	GXInt frame = (GXInt)( animPos * animData->Animations[ 0 ].NumRawFrames );
	if ( frame == animData->Animations[ 0 ].NumRawFrames ) frame--;

	*rot = i >= animData->totalbones ? 0 : &animData->RawKeys[ GET_RAW_FRAME ( 0, frame ) + i ].Orientation;
	*loc = i >= animData->totalbones ? 0 : &animData->RawKeys[ GET_RAW_FRAME ( 0, frame ) + i ].Position;

	#undef GET_RAW_FRAME
}

GXVoid GXAnimSolverPlayer::Update ( GXFloat delta )
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

GXVoid GXAnimSolverPlayer::SetAnimationSequence ( const GXSkeletalMeshAnimationInfo* animData )
{
	GXSafeDelete ( finder );
	finder = new GXBoneFinder ( *animData );

	this->animData = animData; 
	delta2PartFartor = (GXFloat)animData->Animations[ 0 ].FPS / (GXFloat)animData->Animations[ 0 ].NumRawFrames;
}

GXVoid GXAnimSolverPlayer::SetAnimationMultiplier ( GXFloat multiplier )
{
	this->multiplier = multiplier;
}