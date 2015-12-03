//version 1.0

#include "GXMSkeletalAppleJack.h"


GXMSkeletalAppleJack::GXMSkeletalAppleJack ()
{
	Load3DModel ();
	InitCharacterContent ();
}

GXMSkeletalAppleJack::~GXMSkeletalAppleJack ()
{
	/*for ( short i = 0; i < GX_RAINBOW_DASH_TOTAL_ANIM; i++ )
	{
		free ( animations [ i ]->Bones );
		free ( animations [ i ]->Animations );
		free ( animations [ i ]->RawKeys );
	}*/
}

GXVoid GXMSkeletalAppleJack::Load3DModel ()
{
	GXLoadPSK ( L"../3D Models/AppleJack.PSK", &skminfo );
	animations [ GX_APPLE_JACK_ANIM_IDLE ] = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Rainbow_Anim_Idle.psa", animations [ GX_APPLE_JACK_ANIM_IDLE ] );
	animations [ GX_APPLE_JACK_ANIM_WALK_FORWARD ] = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Rainbow_Anim_Walk.psa", animations [ GX_APPLE_JACK_ANIM_WALK_FORWARD ] );
	animations [ GX_APPLE_JACK_ANIM_WALK_BACKWARD ] = animations [ GX_APPLE_JACK_ANIM_WALK_FORWARD ];
	SetAnimation ( GX_APPLE_JACK_ANIM_IDLE );
	GXLoadMTR ( L"../Materials/Matrix Core Animation/AppleJack.mtr", mat_info );
	GXGetTexture ( mat_info.textures [ 0 ] );
	GXGetTexture ( mat_info.textures [ 1 ] );
}

GXVoid GXMSkeletalAppleJack::Pick ()
{
	//NOTHING
}

GXVoid GXMSkeletalAppleJack::SetAnimation ( GXUShort anim_id )
{
	currenAnimation = anim_id;
	switch ( currenAnimation )
	{
		case GX_APPLE_JACK_ANIM_RUN:
		case GX_APPLE_JACK_ANIM_WALK_FORWARD:
			skmaniminfo = animations [ GX_APPLE_JACK_ANIM_WALK_FORWARD];
			frameStep = 1;
			currentFrame = 0;
			delayByFrame = GXDword ( 1000 / skmaniminfo->Animations [ 0 ].FPS );
		break;

		case GX_APPLE_JACK_ANIM_WALK_BACKWARD:
			skmaniminfo = animations [ GX_APPLE_JACK_ANIM_WALK_BACKWARD ];
			frameStep = -1;
			currentFrame = 50;
			delayByFrame = GXDword ( 1000 / skmaniminfo->Animations [ 0 ].FPS );
		break;

		case GX_APPLE_JACK_ANIM_IDLE:
			skmaniminfo = animations [ GX_APPLE_JACK_ANIM_IDLE ];
			frameStep = 1;
			currentFrame = 360;
			delayByFrame = GXDword ( 1000 / skmaniminfo->Animations [ 0 ].FPS );
		break;

		default:
			skmaniminfo = animations [ GX_APPLE_JACK_ANIM_IDLE ];
		break;
	}
	lasttime = ( GXDword )0;
}

GXVoid GXMSkeletalAppleJack::Animate ()
{
	if ( lasttime < delayByFrame ) return;
	currentFrame += frameStep;
	lasttime = 0;
	static int stopframe = 479;
	switch ( currenAnimation )
	{
		case GX_APPLE_JACK_ANIM_RUN:
		case GX_APPLE_JACK_ANIM_WALK_FORWARD:
			if ( currentFrame >= 50 ) 
				currentFrame = 0;
		break;

		case GX_APPLE_JACK_ANIM_WALK_BACKWARD:
			if ( currentFrame <= 0 ) 
				currentFrame = 50;
		break;

		case GX_APPLE_JACK_ANIM_IDLE:
			if ( currentFrame >= stopframe ) 
			{
				char next = rand() % 10;
				switch ( next )
				{
					case 0:
					case 1:
					case 2:
					case 3:
						currentFrame = 360;
						stopframe = 479;
					break;

					case 4:
						currentFrame = 480;
						stopframe = 679;
					break;

					case 5:
					case 6:
						currentFrame = 0;
						stopframe = 100;
					break;

					case 7:
					case 8:
						currentFrame = 100;
						stopframe = 276;
					break;

					case 9:
						currentFrame = 276;
						stopframe = 360;
					break;
				}
			}
		break;
	}
	InitPose ( 0,  currentFrame );
	bNeedUpdateSkeleton = GX_TRUE;
}

GXVoid GXMSkeletalAppleJack::SetLocation ( GXFloat* Loc )
{
	GXMesh::SetLocation ( Loc );
}

GXVoid GXMSkeletalAppleJack::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXMesh::SetLocation ( x, y, z );
}