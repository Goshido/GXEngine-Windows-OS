//version 1.0

#include "GXMSkeletalTwilightSparkle.h"


GXMSkeletalTwilightSparkle::GXMSkeletalTwilightSparkle ()
{
	Load3DModel ();
	InitCharacterContent ();
}

GXMSkeletalTwilightSparkle::~GXMSkeletalTwilightSparkle ()
{
	for ( GXShort i = 0; i < GX_TWILIGHT_SPARKLE_TOTAL_ANIM; i++ )
	{
		free ( animations [ i ]->Bones );
		free ( animations [ i ]->Animations );
		free ( animations [ i ]->RawKeys );
		delete animations [ i ];
	}
}

GXVoid GXMSkeletalTwilightSparkle::Load3DModel ()
{
	GXLoadPSK ( L"../3D Models/Twily.PSK", &skminfo );
	animations [ GX_TWILIGHT_SPARKLE_ANIM_IDLE ] = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Twily_Anim_Fun_Jump.psa", animations [ GX_TWILIGHT_SPARKLE_ANIM_IDLE ] );
	animations [ GX_TWILIGHT_SPARKLE_ANIM_WALK_FORWARD ] = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Twily_Anim_Walk.psa", animations [ GX_TWILIGHT_SPARKLE_ANIM_WALK_FORWARD ] );
	animations [ GX_TWILIGHT_SPARKLE_ANIM_RUN ] = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Twily_Anim_Run.psa", animations [ GX_TWILIGHT_SPARKLE_ANIM_RUN ] );
	SetAnimation ( GX_TWILIGHT_SPARKLE_ANIM_IDLE );
	GXLoadMTR ( L"../Materials/Matrix Core Animation/TwilightSparkle.mtr", mat_info );
	GXGetTexture ( mat_info.textures [ 0 ] );
	GXGetTexture ( mat_info.textures [ 1 ] );
}

GXVoid GXMSkeletalTwilightSparkle::Pick ()
{
	//NOTHING
}

GXVoid GXMSkeletalTwilightSparkle::SetAnimation ( GXUShort anim_id )
{
	currenAnimation = anim_id;
	switch ( currenAnimation )
	{
		case GX_TWILIGHT_SPARKLE_ANIM_IDLE:
			skmaniminfo = animations [ GX_TWILIGHT_SPARKLE_ANIM_IDLE ];
			frameStep = 1;
			currentFrame = 0;
			delayByFrame = GXDword ( 1000 / skmaniminfo->Animations [ 0 ].FPS ) / 2;
		break;

		case GX_TWILIGHT_SPARKLE_ANIM_WALK_FORWARD:
			skmaniminfo = animations [ GX_TWILIGHT_SPARKLE_ANIM_WALK_FORWARD ];
			frameStep = 1;
			currentFrame = 0;
			delayByFrame = GXDword ( 1000 / skmaniminfo->Animations [ 0 ].FPS );
		break;

		case GX_TWILIGHT_SPARKLE_ANIM_WALK_BACKWARD:
			skmaniminfo = animations [ GX_TWILIGHT_SPARKLE_ANIM_WALK_FORWARD ];
			frameStep = -1;
			currentFrame = skmaniminfo->totalrawkeys / skmaniminfo->totalbones - 1;
			delayByFrame = GXDword ( 1000 / skmaniminfo->Animations [ 0 ].FPS );
		break;

		case GX_TWILIGHT_SPARKLE_ANIM_RUN:
			skmaniminfo = animations [ GX_TWILIGHT_SPARKLE_ANIM_RUN ];
			frameStep = 1;
			currentFrame = skmaniminfo->totalrawkeys / skmaniminfo->totalbones - 1;
			delayByFrame = GXDword ( 1000 / skmaniminfo->Animations [ 0 ].FPS );
		break;
	
		default:
			skmaniminfo = animations [ GX_TWILIGHT_SPARKLE_ANIM_IDLE ];
		break;
	}
	lasttime = ( GXDword )0;
}

GXVoid GXMSkeletalTwilightSparkle::Animate ()
{
	if ( lasttime < delayByFrame ) return;
	currentFrame += frameStep;
	lasttime = 0;
	switch ( currenAnimation )
	{
		case GX_TWILIGHT_SPARKLE_ANIM_IDLE:
			if ( currentFrame >= ( GXInt ( skmaniminfo->totalrawkeys / skmaniminfo->totalbones ) - GXInt ( frameStep ) ) ) 
				currentFrame = 0;
		break;

		case GX_TWILIGHT_SPARKLE_ANIM_WALK_FORWARD:
			if ( currentFrame >= ( GXInt ( skmaniminfo->totalrawkeys / skmaniminfo->totalbones ) - GXInt ( frameStep ) ) ) 
				currentFrame = 0;
		break;

		case GX_TWILIGHT_SPARKLE_ANIM_WALK_BACKWARD:
			if ( currentFrame < 0 ) 
				currentFrame = skmaniminfo->totalrawkeys / skmaniminfo->totalbones - 1;
		break;	

		case GX_TWILIGHT_SPARKLE_ANIM_RUN:
			if ( currentFrame >= ( GXInt ( skmaniminfo->totalrawkeys / skmaniminfo->totalbones ) - GXInt ( frameStep ) ) ) 
				currentFrame = 0;
		break;
	}
	InitPose ( 0,  currentFrame );
	bNeedUpdateSkeleton = GX_TRUE;
}


GXVoid GXMSkeletalTwilightSparkle::SetLocation ( GXFloat* Loc )
{
	GXMesh::SetLocation ( Loc );
}

GXVoid GXMSkeletalTwilightSparkle::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXMesh::SetLocation ( x, y, z );
}