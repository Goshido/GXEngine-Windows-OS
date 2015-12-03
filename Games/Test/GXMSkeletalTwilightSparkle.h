//version 1.0

#ifndef GX_M_SKELETAL_TWILIGHT_SPARKLE
#define GX_M_SKELETAL_TWILIGHT_SPARKLE


#include "GXMSkeletalMLPCharacter.h"

#define GX_TWILIGHT_SPARKLE_TOTAL_ANIM				3
#define GX_TWILIGHT_SPARKLE_ANIM_IDLE				0
#define GX_TWILIGHT_SPARKLE_ANIM_WALK_FORWARD		1
#define GX_TWILIGHT_SPARKLE_ANIM_RUN				2
#define GX_TWILIGHT_SPARKLE_ANIM_WALK_BACKWARD		3


class GXMSkeletalTwilightSparkle : public GXMSkeletalMLPCharacter
{
	protected:
		GXSkeletalMeshAnimationInfo*	animations[ GX_TWILIGHT_SPARKLE_TOTAL_ANIM ];

	public:
		GXMSkeletalTwilightSparkle ();
		virtual ~GXMSkeletalTwilightSparkle ();
		virtual GXVoid SetAnimation ( GXUShort anim_id );
		virtual GXVoid Pick ();
		virtual GXVoid SetLocation ( GXFloat* Loc );
		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid Animate ();
};


#endif	//GX_M_SKELETAL_TWILIGHT_SPARKLE